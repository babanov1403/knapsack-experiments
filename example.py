#!/usr/bin/python3
# -*- coding: utf-8 -*-

import sys


def processInput(inputData):
    lines = inputData.split('\n')

    firstLine = lines[0].split()
    items = int(firstLine[0])
    capacity = int(firstLine[1])

    values = []
    weights = []

    for i in range(1, items+1):
        line = lines[i]
        parts = line.split()

        values.append(int(parts[0]))
        weights.append(int(parts[1]))

    items = len(values)

    return (items, capacity, values, weights)


def stringifyAnswer(value, taken):
    outputData = str(value) + '\n'
    outputData += ' '.join(map(str, taken))
    return outputData


def solveCase(items, capacity, values, weights):
    value = 0
    weight = 0
    taken = []

    for i in range(0, items):
        if weight + weights[i] <= capacity:
            taken.append(i + 1)
            value += values[i]
            weight += weights[i]

    return (value, taken)


def solveIt(inputData):
    # Modify this code to run your optimization algorithm

    # parse the input
    (items, capacity, values, weights) = processInput(inputData)

    # a trivial greedy algorithm for filling the knapsack
    # it takes items in-order until the knapsack is full
    (value, taken) = solveCase(items, capacity, values, weights)

    # prepare the solution in the specified output format
    return stringifyAnswer(value, taken)


if __name__ == '__main__':
    print(solveIt(sys.stdin.read().strip()))
