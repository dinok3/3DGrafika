import math
radius = 1
height = 2
segments = 6
cilidar_vertices_d = []
cilidar_vertices_g = [] 


def calculate_vertex(kut, y):
    z = round(math.sin(math.radians(kut)), 6)
    x = round(math.cos(math.radians(kut)), 6)
    return (x,y,z)

def calculate_normal(x1,x2,x3, y1,y2,y3, z1,z2,z3):
   
    v1 = y1-x1 
    v2 = y2-x2
    v3 = y3-x3

    w1 = z1-x1
    w2 = z2-x2
    w3 = z3-x3

    # v x w
    x = round(v2*w3-v3*w2, 6)
    y = round(v1*w3-v3*w1, 6)
    z = round(v1*w2-v2*w1, 6)
    return (x,y,z)

cilidar_vertices = []
def calculate_cilidar_vertices(segments):
    kut = 360 / segments

    for i in range(0, segments):
        v_d = calculate_vertex(kut*i,0.0)
        
        #vert_norm (next,prev)
        next_v = calculate_vertex(kut*(i+1),0.0)
        prev_v = calculate_vertex(kut*(i-1),0.0)
        vert_norm = calculate_normal(*v_d,*next_v, *prev_v)
        
        #horizont_norm (prev, up)
        v_g = calculate_vertex(kut*i,height)
        hor_norm = calculate_normal(*v_d, *v_g, *prev_v)
        
        cilidar_vertices_d.append([v_d,vert_norm])
        cilidar_vertices_d.append([v_d,hor_norm])

    for i in range(0, segments):
        v_g = calculate_vertex(kut*i,height)
        
        #vert_norm
        next_v = calculate_vertex(kut*(i+1),height)
        prev_v = calculate_vertex(kut*(i-1),height)
        vert_norm = calculate_normal(*v_g,*next_v, *prev_v)

        #horizont_norm (prev, up)
        v_d = calculate_vertex(kut*i,0.0)
        prev_v_d = calculate_vertex(kut*(i-1),0.0)
        hor_norm = calculate_normal(*v_g, *prev_v_d, *v_d)
        
        cilidar_vertices_g.append([v_g,vert_norm])
        cilidar_vertices_g.append([v_g,hor_norm])



    cilidar_vertices.extend(cilidar_vertices_d)
    cilidar_vertices.extend(cilidar_vertices_g)
    return cilidar_vertices


def write_to_file(list_of_values):
    with open("cilindar.obj","w") as file:
        for value in list_of_values:
            vertex = value[0]
            x,y,z = vertex
            file.write(f"v {x:.6f} {y:.6f} {z:.6f}\n") 

        for value in list_of_values:
           normal = value[1]
           x,y,z = normal
           file.write(f"vn {x:.6f} {y:.6f} {z:.6f}\n")

        


if __name__ == "__main__":
    values = calculate_cilidar_vertices(segments)
    write_to_file(values)
