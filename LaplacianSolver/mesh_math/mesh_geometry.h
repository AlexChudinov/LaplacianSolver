#ifndef MESH_GEOMETRY_H
#define MESH_GEOMETRY_H

#include <vector>
#include <utility>
#include <map>

#include "linear_algebra/vectortemplate.h"
#include "graph.h"

template<typename Float, typename label> class mesh_geometry;

/**
 * Mesh connectivity and node space positions
 */
template<typename Float, typename label>
class mesh_geometry
{
public:
    using string         = std::string;
    using graph          = data_structs::graph<label>;
    using vector3f       = math::vector_c<Float, 3>;
    using node_positions = std::vector<vector3f>;
    using box3D          = std::pair<vector3f, vector3f>;
    using label_vector   = std::vector<label>;
    using boundary_region_type = std::pair<graph, label_vector>; //label_vector creates boundary labels into the mesh mapping
    using boundary_regions_list = std::map<string, boundary_region_type>;
    using field          = std::vector<Float>;
    using fields_list    = std::map<string, field>;
    using field_entry    = std::pair<string, field>;
    using boundary_entry = std::pair<string, boundary_region_type>;

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
    boundary_regions_list boundary_mesh_;
    fields_list fields_;
    node_types_list node_types_;

public:
    mesh_geometry(const graph& g, const node_positions& np)
        : mesh_connectivity_(g),
          node_positions_(np),
          node_types_(np.size(), INNER_POINT)
    { assert(g.size() == np.size()); }

    /**
     * Returns all mesh connections for OpenGL drawing with GL_LINES flag
     */
    node_positions mesh_connections() const
    {
        node_positions res(mesh_connectivity_.connections_number()*2); //Each connection contains two vertices
        size_t counter = 0;

        auto observer = [&res, &counter, this](size_t i, size_t j)
        {
            res[counter++] = node_positions_[i];
            res[counter++] = node_positions_[j];
        };

        mesh_connectivity_.iterate_over_unique_connections(observer);

        return res;
    }

    /**
     * Returns a number of nodes in the mesh
     */
    inline size_t nodes_number() const { return node_positions_.size(); }

    /**
     * Returns minimal box that contains whole mesh
     */
    box3D containing_box() const
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
    inline bool check_boundary(const graph& mesh) const
    { return mesh.size() <= this->nodes_number(); }
    /**
     * Checks if the mesh has a boundary with a such name
     */
    inline bool is_boundary(const string& name) const
    { return boundary_mesh_.lower_bound(name) != boundary_mesh_.end(); }
    /**
     * Checks if the mesh contains field with a such name
     */
    inline bool is_field(const string& name) const
    { return fields_.lower_bound(name) != boundary_mesh_.end(); }

    /**
     * Adds new boundary returns true if it is ok
     */
    bool add_boundary(const string& name, const graph& mesh, NODE_TYPE type = BOUNDARY_FIXED_VALUE)
    {
        //If the mesh already has this boundary
        if(this->is_boundary(name)) return false;
        //If the mesh is not fitted with the boundary
        if(!(this->check_boundary(mesh))) return false;

        const boundary_region_type& br =
                (this->boundary_mesh_[name] = mesh.remove_empty_connections());

        auto observer = [this,type,br](label node_label)
        {
            this->node_types_[br.second[node_label]] = type;
        };

        br.first.dfs_iterative(observer);

        return true;
    }

    /**
     * Sets boundary field value
     */
    bool set_boundary_val
    (
            const string& boundary_name,
            const string& field_name,
            const Float& val)
    {
        //If there is no such a boundary
        if(!(this->is_boundary(boundary_name))) return false;

        //If there is no such a field
        if(!(this->is_field(field_name))) return false;

        const field& f = this->fields_[field_name];
        const boundary_region_type& br = this->boundary_mesh_[boundary_name];

        auto observer = [val, f, br](label node_label)
        {
            f[br.second[node_label]] = val;
        };

        br.first.dfs_iterative(observer);

        return true;
    }

    /**
     * Sets boundary type
     */
    bool set_boundary_type(const string& name, NODE_TYPE type)
    {
        if(!(this->is_boundary(name))) return false;

        const boundary_region_type& br = this->boundary_mesh_[name];

        auto observer = [this, type, br](label node_label)
        {
            this->node_types_[br.second[node_label]] = type;
        };

        br.first.dfs_iterative(observer);

        return true;
    }
    /**
     * Gets boundary type
     * Note: If nodes of the boundary contain different types the function returns UNKNOWN type
     */
    NODE_TYPE get_boundary_type(const string& name) const
    {
        if(!(this->is_boundary(name))) return UNKNOWN;

        NODE_TYPE type;
        const boundary_region_type& br = this->boundary_mesh_.at(name);
        type = this->node_types_[br.second[0]];

        br.first.dfs_iterative([&type, this, br](label node_label)
        {
            if(br.second[node_label] != 0 && type != this->node_types_[br.second[node_label]])
                type = UNKNOWN;
        });

        return type;
    }
    /**
     * Adds field entry to the mesh
     */
    bool add_field(const string& name, const field& f)
    {
        //Field already exists
        if(this->is_field(name)) return false;
        //Field is not corresponds to the mesh
        if(this->node_positions_.size() != f.size()) return false;

       this->fields_[name] = f;
        return true;
    }

    /**
     * Iterates over the all boundaries
     */
    template<typename Observer>
    void iterate_over_boundaries(Observer observer) const
    {
        for(const boundary_entry& b : this->boundary_mesh_)
            observer(b);
    }

    /**
     * Iterates over the all fields
     */
    template<typename Observer>
    void iterate_over_fields(Observer observer) const
    {
        for(const field_entry& f : this->fields_)
            observer(f);
    }

    /**
     * Returns field refs
     */
    field& field_ref(const string& name)
    {
        return fields_.at(name);
    }
    const field& field_cref(const string& name) const
    {
        return fields_.at(name);
    }
};

#endif // MESH_GEOMETRY_H
