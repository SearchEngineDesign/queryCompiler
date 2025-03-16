#include "Crawler/crawler/Crawl.h"
#include "parser/HtmlParser.h"
#include <pthread.h>
#include "utils/vector.h"
#include "Crawler/crawler/frontier.h"
#include "Crawler/indexParser/index.h"

#include <string>

#include "frontier/frontier.h"

using namespace utils;

struct crawlArg {
    ParsedUrl url;
    char *buffer;
    size_t pageSize;
};

void *crawl_func(void *arg) {
    crawlArg *cArg = (crawlArg *) arg;
    ParsedUrl url = cArg->url;
    char *buffer = cArg->buffer;
    size_t pageSize = cArg->pageSize;

    crawl(url, buffer, pageSize);
 
    // return (void *) cArg;

    pthread_exit( ( void* ) cArg );
}

struct parserArg {
    Index* index;
    char *buffer;
    size_t pageSize;
};

void *parseFunc(void *arg) {
    parserArg* pArg = (parserArg *) arg;

    HtmlParser parser(pArg->buffer, pArg->pageSize);
    //TODO: acquire lock for index
    pArg->index->addDocument(parser);

    pthread_exit( ( void* ) pArg );
}


int main() {

    static const int NUM_THREADS = 1;
    static const int MAX_PAGE_SIZE = 2000000;
    
    static const int ERROR_RATE = 0.0001; // 0.01% error rate for bloom filter
    static const int NUM_OBJECTS = 1000000; // estimated number of objects for bloom filter

    ThreadSafeFrontier frontier(NUM_OBJECTS, ERROR_RATE);
    
    // !WARN IDK IF YOU CAN RESIZE THREADS SO MAKE SURETO RESERVE
    utils::vector<pthread_t> threads;
    threads.reserve(NUM_THREADS);



    

    std::string url = "https://www.google.com";
    char buffer[MAX_PAGE_SIZE]; //don't use a buffer! write to a mapped file or other data structure
    size_t pageSize;
    ParsedUrl purl(url);


    frontier.insert(url);


    crawlArg* cArg;
    cArg->url = purl;
    cArg->buffer = buffer;
    cArg->pageSize = pageSize;

    for (size_t i = 0; i < NUM_THREADS; i++)
    {
        const int rc = pthread_create( &threads[i], NULL, crawl_func, (void*) cArg);

    }
    


    for (size_t i = 0; i < threads.size(); i++)
    {
        pthread_join( threads[i], NULL );
    }
    

    // std::cout << string(buf, 





    return 0;
}