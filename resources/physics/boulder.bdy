# Mesh for a simple boulder
# Vertices
12
	 1.000 0.0000 1.0 # 0
	 0.866  0.499 1.0 # 1
	 0.500  0.866 1.0 # 2
	 0.000  0.999 1.0 # 3
	-0.499  0.866 1.0 # 4
	-0.866  0.500 1.0 # 5
	-0.999  0.000 1.0 # 6
	-0.866 -0.499 1.0 # 7
	-0.500 -0.866 1.0 # 8
	-0.000 -0.999 1.0 # 9
	 0.499 -0.866 1.0 #10
	 0.866 -0.500 1.0 #11

	 
# constraints
14

# outer ring
2  0  1 0.9 distance
2  1  2 0.9 distance
2  2  3 0.9 distance
2  3  4 0.9 distance
2  4  5 0.9 distance
2  5  6 0.9 distance
2  6  7 0.9 distance
2  7  8 0.9 distance
2  8  9 0.9 distance
2  9 10 0.9 distance
2 10 11 0.9 distance
2 11  0 0.9 distance

# cross constraints
2  0  6 0.5 distance
2  3  9 0.5 distance