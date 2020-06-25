OBJECTS := \
	main-with-new-cla.o \
	data-processing.o \
	parse-command-line-args.o \
	print-usage.o \
	read-write.o \
	str-to-int.o

CPPFLAGS := \
	-Wall \
	-Wextra \
	-std=c++11

spa-reader: $(OBJECTS)
	g++ -o spa-reader $(OBJECTS)

# Use implicit rules
main-with-new-cla.o: \
	data-processing.h \
	parse-command-line-args.h \
	print-usage.h \
	read-write.h \
	str-to-int.h

data-processing.o: data-processing.h
parse-command-line-args.o: parse-command-line-args.h
print-usage.o: print-usage.h
read-write.o: read-write.h
str-to-int.o: str-to-int.h

.PHONY: clean
clean:
	rm -f $(OBJECTS)
