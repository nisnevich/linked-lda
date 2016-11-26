cmake_minimum_required(VERSION 3.6)
project(cpp_linked_lda)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    src/ConfigFile.cpp
    src/ConfigFile.h
    src/Corpus-inl.h
    src/Corpus.cpp
    src/Corpus.h
    src/CountTables-inl.h
    src/CountTables.cpp
    src/CountTables.h
    src/Document-inl.h
    src/Document.cpp
    src/Document.h
    src/linkedLDA.cpp
    src/LinkGraph-inl.h
    src/LinkGraph.cpp
    src/LinkGraph.h
    src/MersenneTwister.h
    src/Model.cpp
    src/Model.h
    src/ModelProperties-inl.h
    src/ModelProperties.cpp
    src/ModelProperties.h
    src/MultinomialDistribution.cpp
    src/MultinomialDistribution.h
    src/Utils.h
    src/Vocabulary-inl.h
    src/Vocabulary.cpp
    src/Vocabulary.h
    test/corpus.txt
    test/fullLinks.txt
    test/links.txt
    test/testcorpus.txt
    test/testLinks.txt
    test/vocab.txt
    config.txt)

add_executable(cpp_linked_lda ${SOURCE_FILES})