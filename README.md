# Hammer Primer (beta-test)

Introduction to using the Hammer parser construction toolkit by Meredith L. Patterson

The Hammer repository is at [github.com/UpstandingHackers/hammer](https://github.com/UpstandingHackers/hammer)

# Help us "beta-test" this primer!

These lectures introduce Hammer, a kit for practical construction of
secure parsers in C, C++, and other languages used in production
programming. The point of Hammer is to save the time and effort of 
practical industry programmers who need to deal with parsing
potentially hostile inputs (which, these days, is pretty much everyone
coding any kind of Internet communications).

Hammer needs you to follow a particular programming/design pattern:
the code that validates the inputs (and actually _defines_ what the
valid inputs are) comes first, before any actions on the data parsed
out of these inputs. This is important, because acting on input data
before the entire message has been fully parsed and recognized as
correctly formed is the most common weakness that leads to memory
corruption.

The lectures introduce necessary theory concepts as they go. No
special knowledge of parsing is required, only an interest in how to
do it right, i.e., with the lowest chance of the parser being
exploited by crafted inputs. If you feel that any part of these
lectures can be made more accessible by a brief introduction of a
relevant concept, please let us know!

This is an early release, and yet Hammer has already moved forward
since these lectures were recorded. Whenever today's Hammer works
differently than described in the lectures, annotations and/or comments
have been added to the video.

Please feel free to contribute writeups on any build configuration
problems you encountered and your solutions! Similarly, please feel
free to contribute examples of your own parsers.


# Table of Contents

  * [Lecture 1 - Introduction](#lecture-1---introduction)
  * [Lecture 2 - JSON/HTTP](#lecture-2---jsonhttp)
  * [Lecture 3 - JSON RPC](#lecture-3---json-rpc)
  * [Lecture 4 - Samba Base64 Parsing Crash](#lecture-4---samba-base64-parsing-crash)
  * [Lecture 5 - Authorization](#lecture-5---authorization)
  * [Lecture 6 - State Machines](#lecture-6---state-machines)
  * [Lecture 7 - Languages](#lecture-7---languages)
  * [Lecture 8 - Installing Hammer](#lecture-8---installing-hammer)
  * [Lecture 9 - Why Base64?](#lecture-9---why-base64)
  * [Lecture 10 - Unit Tests](#lecture-10---unit-tests)
  * [Lecture 11 - Base64 Recognizer](#lecture-11---base64-recognizer)
  * [Lecture 12 - Base64 Decoder](#lecture-12---base64-decoder)
  * [Lecture 13 - JSON Parser](#lecture-13---json-parser)

## Lecture 1 - Introduction

**URL**: [https://www.youtube.com/watch?v=7TnZ-ZAGNlw](https://www.youtube.com/watch?v=7TnZ-ZAGNlw&index=1&list=PLMAs0n8Mjs9o2I4KZ14gQr2osDsd3YPoT)

**Abstract**: Parser Combinators and Hammer are introduced, course outline.
System requirements: 
* Linux, or
* OS X with homebrew or macports, or
* Windows with virtual machine

## Lecture 2 - JSON/HTTP

**URL**: [https://www.youtube.com/watch?v=eJnUSDgLiv8](https://www.youtube.com/watch?v=eJnUSDgLiv8&index=2&list=PLMAs0n8Mjs9o2I4KZ14gQr2osDsd3YPoT)

**Abstract**: JSON, JSON RPC introduced. Sent in HTTP POST request, so course will cover it as well.

## Lecture 3 - JSON RPC

**URL**: [https://www.youtube.com/watch?v=dyuxoh2Jcm4](https://www.youtube.com/watch?v=dyuxoh2Jcm4&list=PLMAs0n8Mjs9o2I4KZ14gQr2osDsd3YPoT&index=3)

**Abstract**: More detailed look at JSON RPC.

## Lecture 4 - Samba Base64 Parsing Crash

**URL**: [https://www.youtube.com/watch?v=q6dkb87CBC4](https://www.youtube.com/watch?v=q6dkb87CBC4&index=4&list=PLMAs0n8Mjs9o2I4KZ14gQr2osDsd3YPoT)

**Abstract**: Installation of samba and swat exploit demonstrated. 

## Lecture 5 - Authorization

**URL**: [https://www.youtube.com/watch?v=8ITvFT6kaPw](https://www.youtube.com/watch?v=8ITvFT6kaPw&list=PLMAs0n8Mjs9o2I4KZ14gQr2osDsd3YPoT&index=5)

**Abstract**: Authorization as a state machine.

## Lecture 6 - State Machines

**URL**: [https://www.youtube.com/watch?v=7_biPaRyX20](https://www.youtube.com/watch?v=7_biPaRyX20&index=6&list=PLMAs0n8Mjs9o2I4KZ14gQr2osDsd3YPoT)

**Abstract**: More detailed look at State Machines.

## Lecture 7 - Languages

**URL**: [https://www.youtube.com/watch?v=PzUKSID9aEI](https://www.youtube.com/watch?v=PzUKSID9aEI&index=7&list=PLMAs0n8Mjs9o2I4KZ14gQr2osDsd3YPoT)

**Abstract**: HTTP Basic Authentication described.

## Lecture 8 - Installing Hammer

**URL**: [https://www.youtube.com/watch?v=_luX3uQumG4](https://www.youtube.com/watch?v=_luX3uQumG4&list=PLMAs0n8Mjs9o2I4KZ14gQr2osDsd3YPoT&index=8)

**Abstract**: How to install hammer and its dependencies.

## Lecture 9 - Why Base64?

**URL**: [https://www.youtube.com/watch?v=tB7jRVM3Wf8](https://www.youtube.com/watch?v=tB7jRVM3Wf8&index=9&list=PLMAs0n8Mjs9o2I4KZ14gQr2osDsd3YPoT)

**Abstract**: Base64 encoding described, rules of base64. Recognizer of base64 will be created by coding the rules.

## Lecture 10 - Unit Tests

**URL**: [https://www.youtube.com/watch?v=WLl5vPCBMwU](https://www.youtube.com/watch?v=WLl5vPCBMwU&list=PLMAs0n8Mjs9o2I4KZ14gQr2osDsd3YPoT&index=10)

**Abstract**: Base64 recognizer boilerplace and unit tests, how to compile a program with hammer.

**Files** in lecture_10/
* [base64.c](lecture_10/base64.c)
* [Makefile](lecture_10/Makefile)
* [test_suite.h](lecture_10/test_suite.h)

## Lecture 11 - Base64 Recognizer

**Corrections:**

**URL**: [https://www.youtube.com/watch?v=QYZxYjXUV0U](https://www.youtube.com/watch?v=QYZxYjXUV0U&list=PLMAs0n8Mjs9o2I4KZ14gQr2osDsd3YPoT&index=11)

**Abstract**: Turning the Base64 alphabet rules into a code description.

**Files** in lecture_11/
* [base64.c](lecture_11/base64.c)
* [Makefile](lecture_11/Makefile)
* [test_suite.h](lecture_11/test_suite.h)

## Lecture 12 - Base64 Decoder

**URL**: [https://www.youtube.com/watch?v=3LjDdxkcY9g](https://www.youtube.com/watch?v=3LjDdxkcY9g&index=12&list=PLMAs0n8Mjs9o2I4KZ14gQr2osDsd3YPoT)

**Abstract**: Converting the Base64 Recognizer into a Decoder.

**Files** in lecture_12/
* [base64.c](lecture_12/base64.c)
* [Makefile](lecture_12/Makefile)
* [test_suite.h](lecture_12/test_suite.h)

**Changes in Hammer** since recording time: 
3:50 -- `HAction` function signatures are now:
`HParsedToken* func(const HParseResult *p, void *user_data);`
That is, `HAction` functions must also take a `user_data` whether or not they use it.


## Lecture 13 - JSON Parser

**URL**: [https://www.youtube.com/watch?v=2OVvBoOGaL0](https://www.youtube.com/watch?v=2OVvBoOGaL0&index=13&list=PLMAs0n8Mjs9o2I4KZ14gQr2osDsd3YPoT)

**Abstract**: Creating a JSON Parser.

**Notes**:
13:43 -- `act_json_object` is changed to `act_json_object_main` abruptly. Ignore this, `act_json_object` is correct.

**Files** in lecture_13/
* [json.c](lecture_13/json.c)
* [json.h](lecture_13/json.h)
* [array.json](lecture_13/array.json)
* [object.json](lecture_13/object.json)
* [Makefile](lecture_13/Makefile)
* [test_suite.h](lecture_13/test_suite.h)
