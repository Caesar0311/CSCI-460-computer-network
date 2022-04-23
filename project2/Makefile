#Makefile for CSCI 460 Project 2


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



all: ${BINARY}/ftpserver  ${TESTS}/${BINARY}/ftpservertest



${BINARY}/ftpserver: ${BUILDS}/ftp_server.o ${BUILDS}/ftp_server_connection_listener.o \
		${BUILDS}/ftp_server_net_util.o	${BUILDS}/ftp_server_string_util.o ${BUILDS}/ftp_server_connection.o \
		${BUILDS}/ftp_server_session.o ${BUILDS}/ftp_server_request.o ${BUILDS}/ftp_server_passive.o \
		${BUILDS}/ftp_server_nlist.o ${BUILDS}/ftp_server_retrieve.o
	${CC} ${WFLAG} ${DFLAG} -o ${BINARY}/ftpserver ${BUILDS}/ftp_server.o ${BUILDS}/ftp_server_connection_listener.o \
	${BUILDS}/ftp_server_net_util.o	${BUILDS}/ftp_server_string_util.o ${BUILDS}/ftp_server_connection.o \
	${BUILDS}/ftp_server_session.o ${BUILDS}/ftp_server_request.o ${BUILDS}/ftp_server_passive.o \
	${BUILDS}/ftp_server_nlist.o ${BUILDS}/ftp_server_retrieve.o



${TESTS}/${BINARY}/ftpservertest: ${TESTS}/${BUILDS}/ftp_server_test.o ${TESTS}/${BUILDS}/ftp_server_test_net_util.o \
		${BUILDS}/ftp_server_string_util.o \
		${BUILDS}/ftp_server_net_util.o \
		${BUILDS}/ftp_server_connection_listener.o \
		${BUILDS}/ftp_server_connection.o \
		${BUILDS}/ftp_server_passive.o \
		${BUILDS}/ftp_server_session.o \
		${BUILDS}/ftp_server_request.o \
		${BUILDS}/ftp_server_nlist.o \
		${BUILDS}/ftp_server_retrieve.o \
		${TESTS}/${BUILDS}/ftp_client_test_connection.o \
		${TESTS}/${BUILDS}/ftp_client_test_control.o \
		${TESTS}/${BUILDS}/ftp_client_test_command.o
	${CC} ${WFLAG} -lboost_unit_test_framework -o ${TESTS}/${BINARY}/ftpservertest \
	${BUILDS}/ftp_server_string_util.o \
	${BUILDS}/ftp_server_net_util.o \
	${BUILDS}/ftp_server_connection_listener.o \
	${BUILDS}/ftp_server_connection.o \
	${BUILDS}/ftp_server_passive.o \
	${BUILDS}/ftp_server_session.o \
	${BUILDS}/ftp_server_request.o \
	${BUILDS}/ftp_server_nlist.o \
	${BUILDS}/ftp_server_retrieve.o \
	${TESTS}/${BUILDS}/ftp_server_test.o \
	${TESTS}/${BUILDS}/ftp_server_test_net_util.o \
	${TESTS}/${BUILDS}/ftp_client_test_connection.o \
	${TESTS}/${BUILDS}/ftp_client_test_control.o \
	${TESTS}/${BUILDS}/ftp_client_test_command.o


${BUILDS}/ftp_server.o: ${SOURCES}/ftp_server.cpp
	${CC} ${WFLAG} ${DFLAG} ${CFLAG} -I${INCLUDES} -o ${BUILDS}/ftp_server.o ${SOURCES}/ftp_server.cpp

${BUILDS}/ftp_server_connection_listener.o: ${SOURCES}/ftp_server_connection_listener.cpp
	${CC} ${WFLAG} ${DFLAG} ${CFLAG} -I${INCLUDES} -o ${BUILDS}/ftp_server_connection_listener.o \
	${SOURCES}/ftp_server_connection_listener.cpp

${BUILDS}/ftp_server_connection.o: ${SOURCES}/ftp_server_connection.cpp
	${CC} ${WFLAG} ${DFLAG} ${CFLAG} -I${INCLUDES} -o ${BUILDS}/ftp_server_connection.o ${SOURCES}/ftp_server_connection.cpp

${BUILDS}/ftp_server_session.o: ${SOURCES}/ftp_server_session.cpp
	${CC} ${WFLAG} ${DFLAG} ${CFLAG} -I${INCLUDES} -o ${BUILDS}/ftp_server_session.o ${SOURCES}/ftp_server_session.cpp

${BUILDS}/ftp_server_net_util.o: ${SOURCES}/ftp_server_net_util.cpp
	${CC} ${WFLAG} ${DFLAG} ${CFLAG} -I${INCLUDES} -o ${BUILDS}/ftp_server_net_util.o ${SOURCES}/ftp_server_net_util.cpp

${BUILDS}/ftp_server_string_util.o: ${SOURCES}/ftp_server_string_util.cpp
	${CC} ${WFLAG} ${DFLAG} ${CFLAG} -I${INCLUDES} -o ${BUILDS}/ftp_server_string_util.o ${SOURCES}/ftp_server_string_util.cpp

${BUILDS}/ftp_server_request.o: ${SOURCES}/ftp_server_request.cpp
	${CC} ${WFLAG} ${DFLAG} ${CFLAG} -I${INCLUDES} -o ${BUILDS}/ftp_server_request.o ${SOURCES}/ftp_server_request.cpp

${BUILDS}/ftp_server_passive.o: ${SOURCES}/ftp_server_passive.cpp
	${CC} ${WFLAG} ${DFLAG} ${CFLAG} -I${INCLUDES} -o ${BUILDS}/ftp_server_passive.o ${SOURCES}/ftp_server_passive.cpp

${BUILDS}/ftp_server_nlist.o: ${SOURCES}/ftp_server_nlist.cpp
	${CC} ${WFLAG} ${DFLAG} ${CFLAG} -I${INCLUDES} -o ${BUILDS}/ftp_server_nlist.o ${SOURCES}/ftp_server_nlist.cpp

${BUILDS}/ftp_server_retrieve.o: ${SOURCES}/ftp_server_retrieve.cpp
	${CC} ${WFLAG} ${DFLAG} ${CFLAG} -I${INCLUDES} -o ${BUILDS}/ftp_server_retrieve.o ${SOURCES}/ftp_server_retrieve.cpp


.PHONY: clean run-server run-example-server run-example-client \
	test test-deliverable1 test-deliverable2 test-deliverable3 \
	test-example test-example-deliverable1 test-example-deliverable2 test-example-deliverable3


clean:
	rm -rf ${BINARY}/*
	rm -rf ${BUILDS}/*
	rm -rf ${TESTS}/${BINARY}/*



run-server: ${BINARY}/ftpserver
	${BINARY}/ftpserver 2020


run-example-server: ${EXAMPLE}/${BINARY}/ftpserver
	${EXAMPLE}/${BINARY}/ftpserver 2020


run-example-client: ${EXAMPLE}/${BINARY}/ftpclient
	${EXAMPLE}/${BINARY}/ftpclient `hostname -I` 2020



test-deliverable1:   ${BINARY}/ftpserver ${TESTS}/${BINARY}/ftpservertest
	${TESTS}/${BINARY}/ftpservertest --log_level=all --run_test=ftp_server_deliverable1


test-deliverable2:   ${BINARY}/ftpserver ${TESTS}/${BINARY}/ftpservertest
	${TESTS}/${BINARY}/ftpservertest --log_level=all --run_test=ftp_server_deliverable2


test-deliverable3:   ${BINARY}/ftpserver ${TESTS}/${BINARY}/ftpservertest
	${TESTS}/${BINARY}/ftpservertest --log_level=all --run_test=ftp_server_deliverable3


test: test-deliverable1 test-deliverable2 test-deliverable3



test-example-deliverable1: ${EXAMPLE}/${BINARY}/ftpserver ${TESTS}/${EXAMPLE}/${BINARY}/ftpservertest
	${TESTS}/${EXAMPLE}/${BINARY}/ftpservertest --log_level=all --run_test=ftp_server_deliverable1


test-example-deliverable2: ${EXAMPLE}/${BINARY}/ftpserver ${TESTS}/${EXAMPLE}/${BINARY}/ftpservertest
	${TESTS}/${EXAMPLE}/${BINARY}/ftpservertest --log_level=all --run_test=ftp_server_deliverable2


test-example-deliverable3: ${EXAMPLE}/${BINARY}/ftpserver ${TESTS}/${EXAMPLE}/${BINARY}/ftpservertest
	${TESTS}/${EXAMPLE}/${BINARY}/ftpservertest --log_level=all --run_test=ftp_server_deliverable3


test-example: test-example-deliverable1 test-example-deliverable2 test-example-deliverable3





