MODEL_FLAGS=-m nano --cpu atmega328
CONFIGURATION=QUALLE

build:
	ano build ${MODEL_FLAGS} --cxxflags=-DCONFIGURATION_${CONFIGURATION}

upload:
	ano upload ${MODEL_FLAGS}

clean:
	ano clean
