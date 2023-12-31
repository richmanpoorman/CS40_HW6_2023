/* 
 *   Name       : CPU.h
 *   Assignment : CS40 Homework 6 (um)
 *   Purpose    : Module responsible for performs the arithmetic and
 *                logical operations of the Universal Machine; fetches,
 *                decodes, and executes instructions from memory
 *   Editors    : Matthew Wong (mwong14), Ivi Fung (sfung02)
 */

#include <stdio.h>

#ifndef UM_CPU_H_
#define UM_CPU_H_

/*
 * Name      : runProgram
 * Purpose   : Emulates the Universal Machine with the given program and 
 *             the given input, output streams
 * Parameter : (FILE *) input   -- The input stream to read input from
 *             (FILE *) output  -- The output stream to write output to
 *             (FILE *) program -- The .um binary file with the program
 * Return    : None
 * Notes     : Runs the entire program, and only ends when it reaches the end
 *             of instructions, or the program halts
 */
void runProgram(FILE *input, FILE *output, FILE *program);

#endif