#include "crawler/crawler.h"
#include "parser/HtmlParser.h"
#include <pthread.h>
#include "utils/vector.h"
#include "frontier/frontier.h"
#include "index/index.h"


#include "utils/string.h"
#include "utils/ThreadSafeQueue.h"

#include "frontier/frontier.h"

#include "threading/ThreadPool.h"

static const float ERROR_RATE = 0.0001; // 0.01% error rate for bloom filter
static const int NUM_OBJECTS = 1000000; // estimated number of objects for bloom filter

static const int DEFAULT_PAGE_SIZE = 200000;


struct crawlerResults {
    ParsedUrl url;
    vector<char> buffer;
    size_t pageSize;

    crawlerResults() : url(""), buffer(), pageSize(0) {}

    crawlerResults(const ParsedUrl& u, const vector<char>& v, size_t p) 
        : url(u), buffer(v), pageSize(p) {}

};


ThreadSafeFrontier frontier(NUM_OBJECTS, ERROR_RATE);
ThreadSafeQueue<crawlerResults> crawlResultsQueue;
Crawler crawler;
IndexWriteHandler indexHandler("/Users/tkmaher/eecs498/SearchEngine/index/chunks");

void crawlUrl(void *arg) {
    // crawlArg *cArg = (crawlArg *) arg;
    // char *buffer = cArg->buffer;
    // size_t pageSize = cArg->pageSize;


    (void) arg;

    ParsedUrl url = ParsedUrl(frontier.getNextURLorWait());
    vector<char> buffer(DEFAULT_PAGE_SIZE);
    size_t pageSize;

    crawler.crawl(url, buffer.data(), pageSize);
 
    crawlerResults cResult(url, buffer, pageSize);
    crawlResultsQueue.put(cResult);

    // return (void *) cArg;

    pthread_exit( arg );
}

void parseFunc(void *arg) {

    crawlerResults cResult = crawlResultsQueue.get();

    HtmlParser parser(cResult.buffer.data(), cResult.pageSize);

    for (const auto& link : parser.links) {
        frontier.insert(link.URL);
    }

    indexHandler.addDocument(parser);

    pthread_exit( ( void* ) arg );
}


int main() {

    static const int NUM_CRAWL_THREADS = 10;
    static const int NUM_PARSER_THREADS = 10;
    static const int MAX_PAGE_SIZE = 2000000;
    
    
    // TODO: IMPLEMENT THREAD POOL

    // !WARN IDK IF YOU CAN RESIZE THREADS SO MAKE SURETO RESERVE
    vector<pthread_t> crawl_threads;
    vector<pthread_t> parse_threads;
    crawl_threads.reserve(NUM_CRAWL_THREADS);
    crawl_threads.reserve(NUM_PARSER_THREADS);


    ThreadPool threadPool(NUM_CRAWL_THREADS);
    
    
    string url = "https://www.wikipedia.org/";
    
    frontier.insert(url);
    
    
    // will run crawlURL and parseFunc 10 times each
    // will probably want to have them in a different thread pool`
    for (size_t i = 0; i < 10; i++)
    {
        threadPool.submit(crawlUrl, (void*) nullptr);
        threadPool.submit(parseFunc, (void*) nullptr);
    }
    

    return 0;
}