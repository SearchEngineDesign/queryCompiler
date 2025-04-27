#pragma once

#include <string>
#include <unordered_set>
#include <array>
#include <algorithm>

#include <cf/searchstring.h>
#include "../index/stemmer/stemmer.h"

class StopWords {
private:
    // Compile-time array of stopwords
    static constexpr std::array<const char*, 127> stopwords_array = {
        "i", "me", "my", "myself", "we", "our", "ours", "ourselves",
        "you", "your", "yours", "yourself", "yourselves", "he", "him", "his",
        "himself", "she", "her", "hers", "herself", "it", "its", "itself",
        "they", "them", "their", "theirs", "themselves", "what", "which",
        "who", "whom", "this", "that", "these", "those", "am", "is", "are",
        "was", "were", "be", "been", "being", "have", "has", "had", "having",
        "do", "does", "did", "doing", "a", "an", "the", "and", "but", "if",
        "or", "because", "as", "until", "while", "of", "at", "by", "for",
        "with", "about", "against", "between", "into", "through", "during",
        "before", "after", "above", "below", "to", "from", "up", "down", "in",
        "out", "on", "off", "over", "under", "again", "further", "then", "once",
        "here", "there", "when", "where", "why", "how", "all", "any", "both",
        "each", "few", "more", "most", "other", "some", "such", "no", "nor",
        "not", "only", "own", "same", "so", "than", "too", "very", "s", "t",
        "can", "will", "just", "don", "should", "now"
    };

    // Compile-time generated unordered_set
    static const std::unordered_set<std::string> stopwords;

public:
    // Check if a word is a stopword
    static bool isStopword(const string& word);
    
    // Get the stopwords set
    static const std::unordered_set<std::string>& getStopwords();
};