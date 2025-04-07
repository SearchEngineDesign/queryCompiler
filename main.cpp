#include "Crawler/crawler.h"
#include "parser/HtmlParser.h"
#include <cstddef>
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

static const int NUM_CRAWL_THREADS = 15;
static const int NUM_PARSER_THREADS = 5;
static const int MAX_PAGE_SIZE = 1000000;

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
IndexWriteHandler indexHandler("./log/chunks");

ThreadPool crawlPool(NUM_CRAWL_THREADS);
ThreadPool parsePool(NUM_PARSER_THREADS);

void crawlRobots(ParsedUrl robots, string base) {
    char * buffer = new char[MAX_PAGE_SIZE];
    size_t pageSize = 0;
    if (!frontier.contains(robots.urlName)) {
        if (!Crawler::crawl(robots, buffer, pageSize)) {
            HtmlParser parser(buffer, pageSize, base);
            for (const auto &goodlink : parser.bodyWords) {
                frontier.insert(goodlink);
            }
            for (const auto &badlink : parser.headWords) {
                frontier.blacklist(badlink);
            }
            buffer = new char[MAX_PAGE_SIZE];
        }
        frontier.blacklist(robots.urlName);
    }
}

void crawlUrl(void *arg) {

    ParsedUrl url = ParsedUrl(frontier.getNextURLorWait());
    char * buffer = new char[MAX_PAGE_SIZE];
    size_t pageSize = 0;

    crawlRobots(url.makeRobots(), url.Service + string("://") + url.Host);

    if (!Crawler::crawl(url, buffer, pageSize)) {
        crawlerResults cResult(url, buffer, pageSize);
        crawlResultsQueue.put(cResult);
    }
    if (parsePool.alive()) {
        parsePool.submit(parseFunc, (void*) nullptr);
        parsePool.wake();
    }
    

    delete[] buffer;
}

void parseFunc(void *arg) {

    crawlerResults cResult = crawlResultsQueue.get();

    HtmlParser parser(cResult.buffer.data(), cResult.pageSize);

    for (const auto &link : parser.links) {
        frontier.insert(link.URL);
        if (crawlPool.alive()) {
            crawlPool.submit(crawlUrl, (void*) nullptr);
            crawlPool.wake();
        }
    }

    if (parser.base.size() > 0) {
        std::cout << parser.base << std::endl; // heartbeat
        switch (indexHandler.addDocument(parser)) {
            case -1:
                // whole frontier write
                std::cout << "Completed write of chunk " << indexHandler.getFilename() << std::endl;
                std::cout << "Writing frontier and bloom filter out to file." << std::endl;
                frontier.writeFrontier(false, 0);
                crawlPool.shutdown();
                parsePool.shutdown();
                break;
            case 1:
                // mini frontier write
                std::cout << "Completed write of chunk " << indexHandler.getFilename() << std::endl;
                std::cout << "Writing truncated frontier out to file" << std::endl;
                frontier.writeFrontier(true, 5);
                break;
            default:
                break;
        }
    }
}

void testreader() {
    IndexReadHandler::testReader("./log/chunks/2");
}

int main(int argc, char * argv[]) {
    //testreader();
    int exit = 0;
    if (argc == 2) {
        std::cout << "Building frontier with specified seedlist." << std::endl;
        if (frontier.buildFrontier(argv[1]) == 1) {
            std::cerr << "Expected input: ./search [path to seedlist]" << std::endl;
            exit = 1;
        }
    } else if (argc == 3) {
        std::cout << "Building frontier with specified seedlist." << std::endl;
        if (frontier.buildFrontier(argv[1]) == 1) {
            std::cerr << "Expected input: ./search [path to seedlist]" << std::endl;
            exit = 1;
        }
        std::cout << "Building bloom filter with specified file." << std::endl;
        if (frontier.buildBloomFilter(argv[2]) == 1) {
            std::cerr << "Expected input: ./search [path to seedlist] [path to bloomfilter]" << std::endl;
            exit = 1;
        }
    } else if (argc > 3) {
        std::cerr << "Expected input: ./search [seedlist]]" << std::endl;
        exit = 1;
    } else {   
        std::cout << "Building frontier with default seedlist." << std::endl;
        if (frontier.buildFrontier("./log/frontier/initlist") == 1)
            exit = 1;
    }
    if (exit == 1) {
        crawlPool.shutdown();
        parsePool.shutdown();
        return 1;
    }
    // will run crawlURL and parseFunc 10 times each
    for (size_t i = 0; i < 10; i++)
    {
        crawlPool.submit(crawlUrl, (void*) nullptr);
        parsePool.submit(parseFunc, (void*) nullptr);
    }    

    return 0;
}
