# tdspsql

tdspsql is a TDS (MSSQL) interface to PostgreSQL. The goal is to be able to replace your MSSQL database with PostgreSQL with no modification to the application.

This is not a way to use MSSQL client tools to access your existing PostgreSQL database - tdspsql uses custom types and namespaces to present something that looks like MSSQL, and existing PostgreSQL types and data is basically invisible.

## Status

Currently tdspsql is very much pre-alpha. It's interesting to play with, but is not yet useful. Right now you can:
- Connect using Microsoft SQL Enterprise Manager (but only via the Query. Object Explorer won't work)
- Connect using tsql from FreeTDS
- CREATE DATABASE
- CREATE TABLE
- INSERT
- SELECT
- CREATE FUNCTION
- CREATE PROCEDURE
- EXEC
- some other stuff

The following shortcomings are known. There are plenty more:
- Error reporting is minimal, and does not match the equivalent MSSQL error.
- Memory leaks are known to exist.
- Very little checking is done of the TDS parsing. Buffer overflows are almost certainly possible.
- No password checking is done
- T-SQL language is only partial implemented. Plenty of clauses are not implemented.
- No specific MSSQL version has been targetted.

# Internals

tdspsql listens on port 1433 for TDS packets, takes the T-SQL statements and turns them into something PostgreSQL can understand, then converts the result back into TDS packets.

## Databases

At the moment all databases for a single MSSQL instance (of which only one is allowed right now anyway) exist as separate schemas/namespaces inside a single PostgreSQL databases. This restriction exists in part because MSSQL allows cross-database queries while PostgreSQL does not. Implementing MSSQL schemas on top of this becomes very hacky and may not scale well, but it is what it is. The "dbo" schema inside the "master" databases becomes "master.dbo", etc.

## Types

All MSSQL types are implemented as custom PosgreSQL types, even when the existing PostgreSQL type would otherwise be compatible. The internal storage is designed to be compatible with the TDS binary format where possible. Casts between native types and tdspsql types exist where required.

## BackgroundWorker

The main listener is implemented as a BackgroundWorker and runs with PostgreSQL. It listens on port 1433 (this is hardcoded at this time) for TDS connections from an MSSQL client.

## TDS

TDS receiving, parsing, and sending is done here. Some allowance has been made for different versions of the protocol but the protocol isn't well documented so it's hard to know what versions will give good results.

## Language

T-SQL is parsed using flex/bison and then converted into something that PostgreSQL can understand

# Installing

## Prerequisites

Under Debian, at least the following packages are required:
- build-essential
- bison
- flex
- libicu-dev
- libgmp-dev

## Building

"make" from the root directory should build tdspsql.so

## Installing

Copy tdspsql.so somewhere.

update postgresql.conf to include the line:
shared_preload_libraries = '"/path/to/tdspsql.so"'

Restart the postgres service

Create a new database called mssqlserver (this is the name of the default MSSQL instance. Other instances are not supported at this time)

Create a new user sa with full access to the mssqlserver database. There is currently no authentication implemented so the password doesn't matter.

Connect using tsql like "TDSVER=7.1 tsql -S localhost -U sa -P nopassword"

