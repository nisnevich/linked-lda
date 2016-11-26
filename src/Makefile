RELEASE= g++ -O3
PROFILE= g++ -p  

OBJECTS= ConfigFile.o Corpus.o CountTables.o Document.o LinkGraph.o Model.o ModelProperties.o MultinomialDistribution.o Vocabulary.o
OBJECTS_P= ConfigFile_P.o Corpus_P.o CountTables_P.o Document_P.o LinkGraph_P.o Model_P.o ModelProperties_P.o MultinomialDistribution_P.o Vocabulary_P.o
all: $(OBJECTS)
	$(RELEASE) -o linkedLDA $(OBJECTS) linkedLDA.cpp

profile: $(OBJECTS_P)
	$(PROFILE) -o linkedLDA $(OBJECTS) linkedLDA.cpp
	
ConfigFile.o: ConfigFile.cpp ConfigFile.h
	$(RELEASE) -c ConfigFile.cpp

ConfigFile_P.o: ConfigFile.cpp ConfigFile.h
	$(PROFILE) -c ConfigFile.cpp

Document.o: Document.cpp Document.h 
	$(RELEASE) -c Document.cpp

Document_P.o: Document.cpp Document.h 
	$(PROFILE) -c Document.cpp

Corpus.o: Corpus.cpp Corpus.h Corpus-inl.h Document.h Document-inl.h Vocabulary.h Vocabulary-inl.h
	$(RELEASE) -c Corpus.cpp

Corpus_P.o: Corpus.cpp Corpus.h Corpus-inl.h Document.h Document-inl.h Vocabulary.h Vocabulary-inl.h
	$(PROFILE) -c Corpus.cpp

CountTables.o: CountTables.cpp CountTables.h CountTables-inl.h Corpus.h Corpus-inl.h LinkGraph.h LinkGraph-inl.h Utils.h 
	$(RELEASE) -c CountTables.cpp

CountTables_P.o: CountTables.cpp CountTables.h CountTables-inl.h Corpus.h Corpus-inl.h LinkGraph.h LinkGraph-inl.h Utils.h 
	$(PROFILE) -c CountTables.cpp

LinkGraph.o: LinkGraph.cpp LinkGraph.h LinkGraph-inl.h Utils.h MersenneTwister.h
	$(RELEASE) -c LinkGraph.cpp
	
LinkGraph_P.o: LinkGraph.cpp LinkGraph.h LinkGraph-inl.h Utils.h MersenneTwister.h
	$(PROFILE) -c LinkGraph.cpp

ModelProperties.o: ModelProperties.cpp ModelProperties.h ModelProperties-inl.h
	$(RELEASE) -c ModelProperties.cpp	

ModelProperties_P.o: ModelProperties.cpp ModelProperties.h ModelProperties-inl.h
	$(PROFILE) -c ModelProperties.cpp	

Model.o: Model.cpp Model.h ModelProperties.h ModelProperties-inl.h CountTables.h CountTables-inl.h
	$(RELEASE) -c Model.cpp

Model_P.o: Model.cpp Model.h ModelProperties.h ModelProperties-inl.h CountTables.h CountTables-inl.h
	$(PROFILE) -c Model.cpp

MultinomialDistribution.o: MultinomialDistribution.cpp MultinomialDistribution.h Utils.h MersenneTwister.h
	$(RELEASE) -c MultinomialDistribution.cpp

MultinomialDistribution_P.o: MultinomialDistribution.cpp MultinomialDistribution.h Utils.h MersenneTwister.h
	$(PROFILE) -c MultinomialDistribution.cpp

Vocabulary.o: Vocabulary.cpp Vocabulary.h Utils.h
	$(RELEASE) -c Vocabulary.cpp
	
Vocabulary_P.o: Vocabulary.cpp Vocabulary.h Utils.h
	$(PROFILE) -c Vocabulary.cpp

clean:
	-rm -f *.o
