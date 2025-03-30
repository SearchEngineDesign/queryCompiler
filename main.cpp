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

void parseFunc(void *arg);
void crawlFunc(void *arg);

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
IndexWriteHandler indexHandler("/Users/tkmaher/eecs498/SearchEngine/index/chunks");

ThreadPool crawlPool(10);
ThreadPool parsePool(10);

void crawlUrl(void *arg) {
    // crawlArg *cArg = (crawlArg *) arg;
    // char *buffer = cArg->buffer;
    // size_t pageSize = cArg->pageSize;


    (void) arg;

    ParsedUrl url = ParsedUrl(frontier.getNextURLorWait());
    vector<char> buffer(DEFAULT_PAGE_SIZE);
    size_t pageSize;

    Crawler::crawl(url, buffer.data(), pageSize);
 
    crawlerResults cResult(url, buffer, pageSize);
    crawlResultsQueue.put(cResult);

    parsePool.submit(parseFunc, (void*) nullptr);

    // return (void *) cArg;

    //pthread_exit( arg );
}

void parseFunc(void *arg) {

    crawlerResults cResult = crawlResultsQueue.get();

    HtmlParser parser(cResult.buffer.data(), cResult.pageSize);

    for (const auto& link : parser.links) {
        frontier.insert(link.URL);
    }

    indexHandler.addDocument(parser);

    if (!parser.links.empty()) {
        crawlPool.submit(crawlUrl, (void*) nullptr);
    }

    //pthread_exit( ( void* ) arg );
}


int main() {

    static const int NUM_CRAWL_THREADS = 10;
    static const int NUM_PARSER_THREADS = 10;
    static const int MAX_PAGE_SIZE = 2000000;
    
    
    string url = "https://www.wikipedia.org/";
    
    frontier.insert(url);
    
    
    // will run crawlURL and parseFunc 10 times each
    // will probably want to have them in a different thread pool`
    for (size_t i = 0; i < 10; i++)
    {
        crawlPool.submit(crawlUrl, (void*) nullptr);
        parsePool.submit(parseFunc, (void*) nullptr);
    }

    while(true)
    

    return 0;
}