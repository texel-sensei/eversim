# Mesh for a simple boulder
# Vertices
4
	 0 1 1.0 # 0
	 0 0 1.0 # 1
	 1 0 1.0 # 2
	 1 1 1.0 # 3

	 
# constraints
6

# outer ring
2  0  1 0.9 distance
2  1  2 0.9 distance
2  2  3 0.9 distance
2  3  0 0.9 distance

# cross
2  0  2 0.9 distance
2  1  3 0.9 distance