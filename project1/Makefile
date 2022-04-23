#Makefile for CSCI 460 Project 1


CC = g++
WFLAG = -Wall
DFLAG = -ggdb
CFLAG = -c


SOURCES = src
INCLUDES = include
BUILDS = build
BINARY = bin
TESTS = test
EXAMPLE = example



all: ${BINARY}/ftpclient ${TESTS}/${BINARY}/ftpclienttest


${BINARY}/ftpclient: ${BUILDS}/ftp_client.o ${BUILDS}/ftp_client_ui.o ${BUILDS}/ftp_client_session.o \
		${BUILDS}/ftp_client_connection.o ${BUILDS}/ftp_client_command.o
	${CC} ${WFLAG} ${DFLAG} -o ${BINARY}/ftpclient ${BUILDS}/ftp_client.o \
		${BUILDS}/ftp_client_ui.o ${BUILDS}/ftp_client_session.o \
		${BUILDS}/ftp_client_connection.o ${BUILDS}/ftp_client_command.o

${TESTS}/${BINARY}/ftpclienttest: ${TESTS}/${BUILDS}/ftp_client_test.o \
		${TESTS}/${BUILDS}/ftp_client_test_net_util.o \
		${BUILDS}/ftp_client_connection.o \
		${BUILDS}/ftp_client_session.o ${BUILDS}/ftp_client_command.o ${BUILDS}/ftp_client_ui.o
	${CC} ${WFLAG} -lboost_unit_test_framework -o ${TESTS}/${BINARY}/ftpclienttest \
		${TESTS}/${BUILDS}/ftp_client_test.o \
		${TESTS}/${BUILDS}/ftp_client_test_net_util.o \
		${BUILDS}/ftp_client_connection.o ${BUILDS}/ftp_client_session.o ${BUILDS}/ftp_client_command.o \
		${BUILDS}/ftp_client_ui.o


${BUILDS}/ftp_client.o: ${SOURCES}/ftp_client.cpp 
	${CC} ${WFLAG} ${DFLAG} ${CFLAG} -I${INCLUDES} -o ${BUILDS}/ftp_client.o ${SOURCES}/ftp_client.cpp

${BUILDS}/ftp_client_ui.o: ${SOURCES}/ftp_client_ui.cpp ${INCLUDES}/ftp_client_ui.hpp
	${CC} ${WFLAG} ${DFLAG} ${CFLAG} -I${INCLUDES} -o ${BUILDS}/ftp_client_ui.o ${SOURCES}/ftp_client_ui.cpp

${BUILDS}/ftp_client_session.o: ${SOURCES}/ftp_client_session.cpp ${INCLUDES}/ftp_client_session.hpp
	${CC} ${WFLAG} ${DFLAG} ${CFLAG} -I${INCLUDES} -o ${BUILDS}/ftp_client_session.o ${SOURCES}/ftp_client_session.cpp

${BUILDS}/ftp_client_connection.o: ${SOURCES}/ftp_client_connection.cpp ${INCLUDES}/ftp_client_connection.hpp
	${CC} ${WFLAG} ${DFLAG} ${CFLAG} -I${INCLUDES} -o ${BUILDS}/ftp_client_connection.o ${SOURCES}/ftp_client_connection.cpp

${BUILDS}/ftp_client_command.o: ${SOURCES}/ftp_client_command.cpp ${INCLUDES}/ftp_client_command.hpp
	${CC} ${WFLAG} ${DFLAG} ${CFLAG} -I${INCLUDES} -o ${BUILDS}/ftp_client_command.o ${SOURCES}/ftp_client_command.cpp


.PHONY: clean \
	run-client run-example-client run-example-server \
	test test-deliverable1 test-deliverabl2 \
	test-example test-example-deliverable1 test-example-deliverabl2


run-client: ${BINARY}/ftpclient
	${BINARY}/ftpclient `hostname -I` 2020


run-example-client: ${EXAMPLE}/${BINARY}/ftpclient
	${EXAMPLE}/${BINARY}/ftpclient `hostname -I` 2020


run-example-server: ${EXAMPLE}/${BINARY}/ftpserver
	${EXAMPLE}/${BINARY}/ftpserver 2020




test-deliverable1: ${TESTS}/${BINARY}/ftpclienttest ${EXAMPLE}/${BINARY}/ftpserver
	${TESTS}/${BINARY}/ftpclienttest --log_level=all --run_test=ftp_client_deliverable1

test-deliverable2: ${TESTS}/${BINARY}/ftpclienttest ${EXAMPLE}/${BINARY}/ftpserver
	${TESTS}/${BINARY}/ftpclienttest --log_level=all --run_test=ftp_client_deliverable2

test: test-deliverable1 test-deliverable2



test-example-deliverable1: ${TESTS}/${EXAMPLE}/${BINARY}/ftpclienttest ${EXAMPLE}/${BINARY}/ftpserver
	${TESTS}/${EXAMPLE}/${BINARY}/ftpclienttest --log_level=all --run_test=ftp_client_deliverable1
# test-example-deliverable1: ${TESTS}/${EXAMPLE}/${BINARY}/ftpclienttest ${EXAMPLE}/${BINARY}/ftpserver
# 	${TESTS}/${EXAMPLE}/${BINARY}/ftpclienttest --log_level=all --run_test=./example/ftpclient



test-example-deliverable2: ${TESTS}/${EXAMPLE}/${BINARY}/ftpclienttest ${EXAMPLE}/${BINARY}/ftpserver
	${TESTS}/${EXAMPLE}/${BINARY}/ftpclienttest --log_level=all --run_test=ftp_client_deliverable2

test-example: test-example-deliverable1 test-example-deliverable2


clean:
	rm -rf ${BINARY}/* 
	rm -rf ${BUILDS}/*
	rm -rf ${TESTS}/${BINARY}/*

	

