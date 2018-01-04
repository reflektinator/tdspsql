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


