#ifndef MESH_GEOMETRY_H
#define MESH_GEOMETRY_H

#include <map>
#include <numeric>

#include <linearAlgebra\vectorTemplate.h>
#include <data_structs\graph.h>

template<typename Float, typename label> class mesh_geometry;

/**
 * Mesh connectivity and node space positions
 */
template<typename Float, typename label>
class MeshGeom
{
public:
	using string         = std::string;
	using node_labels    = std::vector<label>;
	using vector3f       = math::vector_c<Float, 3>;
	using node_positions = std::vector<vector3f>;
	using graph          = data_structs::graph<label>;
    using box3D          = std::pair<vector3f, vector3f>;
    using label_list	 = std::set<label>;

    using boundaries_list= std::map<string, node_labels>;
	using boundary_entry = std::pair<string, node_labels>;

    /**
     *  Node types of a mesh
     */
    enum NODE_TYPE : uint8_t
    {
        INNER_POINT            = 0x00,
        BOUNDARY_ZERO_GRADIENT = 0x01,
        BOUNDARY_FIXED_VALUE   = 0x02,
        UNKNOWN                = 0xFF ///Some error case
    };

    using node_types_list= std::vector<NODE_TYPE>;

private:
    graph mesh_connectivity_;
    node_positions node_positions_;
    boundaries_list boundary_mesh_;
	node_types_list node_types_;

public:
	MeshGeom(const graph& g, const node_positions& np)
        : mesh_connectivity_(g),
          node_positions_(np),
          node_types_(np.size(), INNER_POINT)
    { 
		if(g.size() != np.size()) throw(std::runtime_error("Graph size and node positions array size mismatch!"));
	}

    /**
     * Returns all mesh connections for OpenGL drawing with GL_LINES flag
     */
    node_positions meshConnections() const
    {
        node_positions res(mesh_connectivity_.connectionsNum()*2); //Each connection contains two vertices
        size_t counter = 0;
        auto observer = [&](size_t i, size_t j)
        {
            res[counter++] = node_positions_[i];
            res[counter++] = node_positions_[j];
        };
        mesh_connectivity_.iterateOverUniqueConnections(observer);
        return res;
    }

    /**
     * Returns a number of nodes in a mesh
     */
    inline size_t size() const { return node_positions_.size(); }

    /**
     * Returns minimal box that contains whole mesh
     */
    box3D box() const
    {
        Float   min_x = node_positions_[0][0],
                min_y = node_positions_[0][1],
                min_z = node_positions_[0][2],
                max_x = node_positions_[0][0],
                max_y = node_positions_[0][1],
                max_z = node_positions_[0][2];

        typename node_positions::const_iterator it = node_positions_.begin() + 1;
        for(; it != node_positions_.end(); ++it)
        {
            min_x = std::min(min_x, (*it)[0]);
            min_y = std::min(min_y, (*it)[1]);
            min_z = std::min(min_z, (*it)[2]);
            max_x = std::max(max_x, (*it)[0]);
            max_y = std::max(max_y, (*it)[1]);
            max_z = std::max(max_z, (*it)[2]);
        }
        return box3D({min_x, min_y, min_z}, {max_x, max_y, max_z});
    }

    /**
     * Checks if the boundary could be attributed to the mesh
     */
    inline bool checkBoundary(const boundary_entry& boundary) const
    { return *std::max_element(boundary.second) > size(); }
    /**
     * Checks if the mesh already has such a boundary
     */
    inline bool isBoundary(const boundary_entry& boundary) const
    { 
		boundaries_list::const_iterator it = boundary_mesh_.lower_bound(boundary.first);
		if (it->first != boundary.first) return false;
		else {
			if (boundary.second.size() != it->second.size()) return false;
			else
			{
				for (size_t i = 0; i < it->second.size(); ++i)
					if (it->second[i] != boundary.second[i]) return false;
				return true;
			}
		}
	}
    /**
     * Adds new boundary returns true if it is ok
     */
    bool addBoundary(const boundary_entry& boundary, NODE_TYPE type = BOUNDARY_FIXED_VALUE)
    {
        //If the mesh already has this boundary
        if(isBoundary(boundary)) return false;
        //If the mesh is not fitted with the boundary
        if(!checkBoundary(boundary)) throw(std::runtime_error("Boundary contains too big labels."));

		boundary_mesh_.insert(boundary);
		for (label l : boundary.second) node_types_[l] = type;

        return true;
    }
    /**
     * Sets boundary type
     */
    bool setBoundaryType(const string& name, NODE_TYPE type)
    {
        if(*boundary_mesh_.lower_bound(name) != name) return false;
		for (label l : boundary_mesh_[name]) node_types_[l] = type;
        return true;
    }
    /**
     * Gets boundary type
     * Note: If nodes of the boundary contain different types the function returns UNKNOWN type
     */
    NODE_TYPE getBoundaryType(const string& name) const
    {
		if (*boundary_mesh_.lower_bound(name) != name) return UNKNOWN;        
		return node_types_[*boundary_mesh_[name].begin()];
    }
};

#endif // MESH_GEOMETRY_H
