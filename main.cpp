#include "Crawler/crawler.h"
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

static const int NUM_CRAWL_THREADS = 15;
static const int NUM_PARSER_THREADS = 5;
static const int MAX_PAGE_SIZE = 2000000;

void parseFunc(void *arg);

struct crawlerResults {
    ParsedUrl url;
    vector<char> buffer;
    size_t pageSize;

    crawlerResults() : url(""), buffer(), pageSize(0) {}

    crawlerResults(const ParsedUrl& u, const char * v, size_t p) 
        : url(u), pageSize(p) {
            for (int i = 0; i < p; ++i)
                buffer.push_back(v[i]);
        }

};

ThreadSafeFrontier frontier(NUM_OBJECTS, ERROR_RATE);
ThreadSafeQueue<crawlerResults> crawlResultsQueue;
<<<<<<< HEAD
// TODO: CHANGE THIS PATH ACCORDINGLY
// IndexWriteHandler indexHandler("/Users/tkmaher/eecs498/SearchEngine/index/chunks");
=======
IndexWriteHandler indexHandler("/Users/tkmaher/eecs498/SearchEngine/log/chunks");
>>>>>>> 9f5bcba1809d780178a2ddaf119046da8f24d46a

// ThreadPool crawlPool(NUM_CRAWL_THREADS);
// ThreadPool parsePool(NUM_PARSER_THREADS);

// void crawlUrl(void *arg) {

<<<<<<< HEAD

//     (void) arg;

//     ParsedUrl url = ParsedUrl(frontier.getNextURLorWait());
//     vector<char> buffer(DEFAULT_PAGE_SIZE);
//     size_t pageSize;
=======
    ParsedUrl url = ParsedUrl(frontier.getNextURLorWait());
    char * buffer = new char[MAX_PAGE_SIZE];
    size_t pageSize;
>>>>>>> 9f5bcba1809d780178a2ddaf119046da8f24d46a

//     std::cout << url.urlName << std::endl;

<<<<<<< HEAD
//     if (!Crawler::crawl(url, buffer.data(), pageSize)) {
//         //buffer.resize(pageSize, '\0');
//         crawlerResults cResult(url, buffer, pageSize);
//         crawlResultsQueue.put(cResult);

//         parsePool.submit(parseFunc, (void*) nullptr);
//         parsePool.wake();
//     }
// }
=======
    if (!Crawler::crawl(url, buffer, pageSize)) {
        crawlerResults cResult(url, buffer, pageSize);
        crawlResultsQueue.put(cResult);

        parsePool.submit(parseFunc, (void*) nullptr);
        parsePool.wake();
    }

    delete[] buffer;
}
>>>>>>> 9f5bcba1809d780178a2ddaf119046da8f24d46a

// void parseFunc(void *arg) {

//     crawlerResults cResult = crawlResultsQueue.get();

//     HtmlParser parser(cResult.buffer.data(), cResult.pageSize);

//     for (const auto& link : parser.links) {
//         frontier.insert(link.URL);
//     }

//     indexHandler.addDocument(parser);

//     if (!frontier.empty()) {
//         crawlPool.submit(crawlUrl, (void*) nullptr);
//         crawlPool.wake();
//     }
// }

// for testing the readibility of the index chunks

void testBlob() {
    HtmlParser parser = HtmlParser();
    for (int i = 0; i < 100; i++)
        parser.bodyWords.emplace_back(string("body"));
    for (int i = 0; i < 20; i++)
        parser.titleWords.emplace_back(string("title"));
    parser.base = "https://baseURL1";
    indexHandler.addDocument(parser);
    indexHandler.index->documents.push_back("https://baseURL2");
    indexHandler.index->documents.push_back("https://baseURL3");
    indexHandler.index->DocumentsInIndex += 2;

    Tuple<string, PostingList> *t1 = indexHandler.index->getDict()->Find("body");
    assert(t1->value.getUseCount() == 100);
    assert(t1->value.getDocCount() == 1);
    Tuple<string, PostingList> *t2 = indexHandler.index->getDict()->Find("@title");
    assert(t2->value.getUseCount() == 20);
    assert(t2->value.getDocCount() == 1);

    indexHandler.WriteIndex();

<<<<<<< HEAD
        if (!Crawler::crawl(url, buffer.data(), pageSize)) {
            buffer.resize(pageSize, '\0');
            HtmlParser parser(buffer.data(), pageSize);
            for (const auto& link : parser.links) {
                frontier.insert(link.URL);
            }

            // indexHandler.addDocument(parser);
            
        }
    }
=======
    IndexReadHandler::testReader(indexHandler);
>>>>>>> 9f5bcba1809d780178a2ddaf119046da8f24d46a
}

int main() {
    
<<<<<<< HEAD
    // TODO: replace with seed list (and periodically write frontier to file)
    string url = "https://en.wikipedia.org/";
    
    frontier.insert(url);
   
    sample();

    // // will run crawlURL and parseFunc 10 times each
    // for (size_t i = 0; i < 10; i++)
    // {
    //     crawlPool.submit(crawlUrl, (void*) nullptr);
    //     parsePool.submit(parseFunc, (void*) nullptr);
    // }    
=======
    frontier.buildFrontier("/Users/tkmaher/eecs498/SearchEngine/log/seedlist.txt");
    
    // will run crawlURL and parseFunc 10 times each
    for (size_t i = 0; i < 10; i++)
    {
        crawlPool.submit(crawlUrl, (void*) nullptr);
        parsePool.submit(parseFunc, (void*) nullptr);
    }    
>>>>>>> 9f5bcba1809d780178a2ddaf119046da8f24d46a

    return 0;
}
