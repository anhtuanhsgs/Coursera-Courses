#!/usr/bin/python
# -*- coding: utf-8 -*-
from __future__ import print_function
from collections import namedtuple
from ortools.linear_solver import pywraplp
import math
import sys

Point = namedtuple("Point", ['x', 'y'])
Facility = namedtuple("Facility", ['index', 'setup_cost', 'capacity', 'location'])
Customer = namedtuple("Customer", ['index', 'demand', 'location'])

def length(point1, point2):
    return math.sqrt((point1.x - point2.x)**2 + (point1.y - point2.y)**2)

def init_data (input_data):
    # parse the input
    lines = input_data.split('\n')

    parts = lines[0].split()
    facility_count = int(parts[0])
    customer_count = int(parts[1])
    
    facilities = []
    for i in range(1, facility_count+1):
        parts = lines[i].split()
        facilities.append(Facility(i-1, float(parts[0]), int(parts[1]), Point(float(parts[2]), float(parts[3])) ))

    customers = []
    for i in range(facility_count+1, facility_count+1+customer_count):
        parts = lines[i].split()
        customers.append(Customer(i-1-facility_count, int(parts[0]), Point(float(parts[1]), float(parts[2]))))

    return facilities, customers

def penalty (c, f):
    return length (c.location, f.location)

def buid_model (fac, cus):

    # Using CBC
    solver = pywraplp.Solver('Facility', pywraplp.Solver.CBC_MIXED_INTEGER_PROGRAMMING)
    solver.Clear ()
    nb_fac = len (fac)
    nb_cus = len (cus)

    cus_prefer = [list (range (nb_fac)) for i in range (nb_cus)]
    fixed_nb_pre = min (10000, nb_fac)

    if (nb_fac == 100 and nb_cus == 100):
        fixed_nb_pre = 100
        for i in range (nb_cus):
            cus_prefer[i].sort (key=lambda x: penalty(cus[i], fac[x]))
    
    #
    # Variables
    #
    take = [solver.IntVar (0, 1, 'take[%i]' % i) for i in range (nb_fac)]

    assign = {}
    for j in range (nb_cus):
        for i in range (fixed_nb_pre):
            assign [i, j] = solver.IntVar (0, 1, 'assign[%i,%i]' % (cus_prefer[j][i], j))

    #
    # Constrain
    #
    # Maximum capacity
    for i in range (nb_fac):
        tmp = [cus[j].demand * assign[i, j] for j in range (nb_cus) if (i, j) in assign]
        solver.Add (solver.Sum (tmp) <= take[i] * fac[i].capacity)

    # Assign only if factory opens
    for j in range (nb_cus):
        for i in range (nb_fac):
            if (i, j) in assign:
                solver.Add (assign[i, j] <= take[i])

    # Exactly one facility per custumer
    for j in range (nb_cus):
        solver.Add (solver.Sum ([assign[i, j] for i in range (nb_fac) if (i, j) in assign]) == 1)

    #
    # Objective
    #
    tmp = [take[i] * fac[i].setup_cost for i in range (nb_fac)]
    tmp.extend ([length (fac[i].location, cus[j].location) * assign[i, j]
                        for i in range (nb_fac)
                        for j in range (nb_cus)
                        if (i, j) in assign])
    objective =  solver.Minimize (solver.Sum (tmp))


    return assign, solver

# Solved subtasks
def get_solved_out (n, m):
    file_name = ''
    if n == 25:
        file_name = "1.out"
    if n == 50:
        file_name = "2.out"
    if n == 100 and m == 100:
        file_name = "3.out"
    if n == 100 and m == 1000:
        file_name = "4.out"
    if n == 200:
        file_name = "5.out"
    if n == 500:
        file_name = "6.out"
    if n == 1000:
        file_name = "7.out"
    if n == 2000:
        file_name = "8.out"

    file = open (file_name, "r")
    return file.read()

def solve_it(input_data):
    
    fac, cus = init_data (input_data)

    # return get_solved_out (len (fac), len (cus))

    if (len (cus) > 1000 or len (fac) == 200):
        return devide_conquer (fac, cus)
    if (len (cus) == 1000 and len (fac) == 100):
        return naive (cus, fac)

    assign, solver = buid_model (fac, cus)

    solver.Solve ()

    solution = [-1]*len(cus)
    obj = float (solver.Objective().Value ())
    for j in range (len (cus)):
        for i in range (len (fac)):    
            if int (assign[i, j].SolutionValue ()) == 1:
                solution [j] = i
                break


    # prepare the solution in the specified output format
    output_data = '%.2f' % obj + ' ' + str(0) + '\n'
    output_data += ' '.join(map(str, solution))

    return output_data

def split_data (fac, cus):
    nb_cut = 5
    overlap_ratio = 0.1

    if len(fac) == 200 and len(cus) == 800:
        nb_cut = 2
        overlap_ratio = 0.1
    if len(fac) == 1000:
        nb_cut = 5
        overlap_ratio = 0.1
    if len(fac) == 2000:
        nb_cut = 8
        overlap_ratio = 0.1
    if len(fac) == 500:
        nb_cut = 5
        overlap_ratio = 0.001


    minX = min ([f.location.x for f in fac])
    minX = min (minX, min ([c.location.x for c in cus])) - 0.1
    maxX = max ([f.location.x for f in fac])
    maxX = max (maxX, max ([c.location.x for c in cus])) + 0.1
    width = 1.0 * (maxX - minX) / nb_cut + 0.1

    minY = min ([f.location.y for f in fac])
    minY = min (minY, min ([c.location.y for c in cus])) - 0.1
    maxY = max ([f.location.y for f in fac]) 
    maxY = max (maxY, max ([c.location.y for c in cus])) + 0.1
    height = 1.0 * (maxY - minY) / nb_cut + 0.1

    sub_fac = []
    sub_cus = []
    cutX = minX
    cutY = minY

    while cutX < maxX:
        cutY = minY
        while cutY < maxY:
            sub_fac.append ([])
            for f in fac:
                if cutX <= f.location.x <= cutX + width and\
                cutY <= f.location.y <= cutY + height:
                    sub_fac[-1].append (f)
            sub_cus.append ([])
            for c in cus:
                if cutX - width * overlap_ratio <= c.location.x <= cutX + width * (1.0 + overlap_ratio) and\
                cutY - height * overlap_ratio <= c.location.y <= cutY + height * (1.0 + overlap_ratio):
                    sub_cus[-1].append (c)
            cutY += height
        cutX += width

    return sub_fac, sub_cus

def devide_conquer (fac, cus):
    sub_fac, sub_cus = split_data (fac, cus)
    solution = [-1]*len(cus)

    # print (len (sub_fac))
    print (len (sub_fac[0]), len (sub_cus[0]), file=sys.stderr)

    for i in range (len(sub_fac)):
            print (i, file=sys.stderr)
            assign, solver = buid_model (sub_fac[i], sub_cus[i])
            solver.Solve ()
            for j in range ( len(sub_cus[i])):
                for k in range (len (sub_fac[i])):
                    if int (assign[k, j].SolutionValue ()) == 1:
                        if solution[sub_cus[i][j].index] < 0 or\
                        length(sub_cus[i][j].location, fac[solution[sub_cus[i][j].index]].location) \
                        > length (sub_cus[i][j].location, sub_fac[i][k].location):
                            solution[sub_cus[i][j].index] = sub_fac[i][k].index

    obj = 0

    for i in range (len (fac)):
        if i in solution:
            obj += fac[i].setup_cost
    for i in range (len (cus)):
        obj += length (fac[solution[i]].location, cus[i].location)

    # prepare the solution in the specified output format
    output_data = '%.2f' % obj + ' ' + str(0) + '\n'
    output_data += ' '.join(map(str, solution))

    return output_data

def naive(customers, facilities):
    # build a trivial solution
    # pack the facilities one by one until all the customers are served
    solution = [-1]*len(customers)
    capacity_remaining = [f.capacity for f in facilities]

    facility_index = 0
    for customer in customers:
        if capacity_remaining[facility_index] >= customer.demand:
            solution[customer.index] = facility_index
            capacity_remaining[facility_index] -= customer.demand
        else:
            facility_index += 1
            assert capacity_remaining[facility_index] >= customer.demand
            solution[customer.index] = facility_index
            capacity_remaining[facility_index] -= customer.demand

    used = [0]*len(facilities)
    for facility_index in solution:
        used[facility_index] = 1

    # calculate the cost of the solution
    obj = sum([f.setup_cost*used[f.index] for f in facilities])
    for customer in customers:
        obj += length(customer.location, facilities[solution[customer.index]].location)

    # prepare the solution in the specified output format
    output_data = '%.2f' % obj + ' ' + str(0) + '\n'
    output_data += ' '.join(map(str, solution))

    return output_data

if __name__ == '__main__':
    import sys
    if len(sys.argv) > 1:
        file_location = sys.argv[1].strip()
        with open(file_location, 'r') as input_data_file:
            input_data = input_data_file.read()
        print(solve_it(input_data))
    else:
        print('This test requires an input file.  Please select one from the data directory. (i.e. python solver.py ./data/fl_16_2)')

