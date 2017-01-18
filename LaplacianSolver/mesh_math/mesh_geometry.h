#ifndef MESH_GEOMETRY_H
#define MESH_GEOMETRY_H

#include <linearAlgebra\vectorTemplate.h>
#include <data_structs\graph.h>

/**
 * Mesh connectivity and node space positions
 */
template<typename Float, typename label>
class mesh_geometry
{
public:
	using string         = std::string;
	using node_labels    = std::vector<label>;
	using vector3f       = math::vector_c<Float, 3>;
	using node_positions = std::vector<vector3f>;
	using graph          = data_structs::graph<label>;
    using box3D          = std::pair<vector3f, vector3f>;
    using label_list	 = std::set<label>;

	//Interpolation coefs
	using interp_coef  = std::pair<label, Float>;
	using interp_coefs = std::vector<interp_coef>;

private:
    graph mesh_connectivity_;
    node_positions node_positions_;
	
	//Numeric limit for floating point precision
	Float m_fEpsilon;
public:
	mesh_geometry(const graph& g, const node_positions& np)
        : mesh_connectivity_(g), node_positions_(np), m_fEpsilon(std::numeric_limits<Float>::epsilon()*100.0)
    { 
		if(g.size() != np.size()) 
			throw(std::runtime_error("Sizes of graph and node positions array mismatch!"));
	}

	//Sets the precision limit
	void eps(size_t fFactor)
	{
		m_fEpsilon = std::numeric_limits<Float>::epsilon()*static_cast<Float>(fFactor);
	}
	//Gets the precision limit
	Float eps() const
	{
		return m_fEpsilon;
	}

    /**
     * Returns all mesh points in the order where points at odd indexes coincides with points at even indexes,
	 * Therefore, if we put the return to a glDrawArray function with GL_LINES it will draw the mesh
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
	 * Returns point3D by a label
	 */
	inline const vector3f& spacePositionOf(label id) const { return node_positions_[id]; }

	/**
	 * Visit neigbour points
	 */
	template<typename visitor>
	void visit_neigbour(label l, visitor V) const
	{
		for (label ll : mesh_connectivity_.getNeighbour(l)) V(ll);
	}

	/**
	 * Search for a clossest point from the start point
	 * Returns the label of that point
	 */
	label find_closest(Float x, Float y, Float z, label start = 0) const
	{
		const vector3f pos{ x, y, z };
		label result = start;
		double minSqrDist = math::sqr(node_positions_[start] - pos);

		if (minSqrDist == 0.0) return start;

		mesh_connectivity_.bfs_iterative(start, 
			[&](label l)->bool 
		{
			double testSqrDist = math::sqr(node_positions_[l] - pos);
			if (testSqrDist <= minSqrDist)
			{
				minSqrDist = testSqrDist;
				result = l;
				return true;
			}
			return false;
		});

		return result;
	}

	//Find closest line if one point is known
	//Assume, that it was found using function find_closest
	label find_line(Float x, Float y, Float z, label start) const
	{
		const vector3f pos = vector3f{ x,y,z };
		label result = *mesh_connectivity_.getNeighbour(start).begin();
		double minSqrDist = math::sqr(node_positions_[result] - pos);

		mesh_connectivity_.bfs_iterative(result,
			[&](label l)->bool
		{
			if (l == start) return true; //Look futher
			double testSqrDist = math::sqr(node_positions_[l] - pos);
			if (testSqrDist <= minSqrDist)
			{
				minSqrDist = testSqrDist;
				result = l;
				return true;
			}
			return false;
		});

		return result;
	}

	//Find closest plane
	//It is assumed that next and start already correspond to a closest line
	label find_plane(Float x, Float y, Float z, label start, label next) const
	{
		const vector3f pos = vector3f{ x,y,z };
		typename label_list::const_iterator it = mesh_connectivity_.getNeighbour(start).begin();
		label result = *it == next ? *(++it) : *it;
		double minSqrDist = math::sqr(node_positions_[result] - pos);

		mesh_connectivity_.bfs_iterative(result,
			[&](label l)->bool
		{
			if (l == start || l == next ||
				math::sqr(math::crossProduct(node_positions_[l] - node_positions_[start], 
					node_positions_[next] - node_positions_[start])) < m_fEpsilon)
				return true; //Look futher if both this is one of the previous or nodes are on a line
			double testSqrDist = math::sqr(node_positions_[l] - pos);
			if (testSqrDist <= minSqrDist)
			{
				minSqrDist = testSqrDist;
				result = l;
				return true;
			}
			return false;
		});

		return result;
	}

	//Find best tetrahedral for the x,y,z point
	//First three nodes of the tet were previously found by the find_closest, 
	//find_line, and find_plane functions
	label find_tet(Float x, Float y, Float z, label start, label next1, label next2) const
	{

	}
};

#endif // MESH_GEOMETRY_H
