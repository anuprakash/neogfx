// i_mesh.hpp
/*
  neogfx C++ GUI Library
  Copyright(C) 2017 Leigh Johnston

  This program is free software: you can redistribute it and / or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

#include <neogfx/neogfx.hpp>
#include <neogfx/core/numerical.hpp>
#include <neogfx/core/geometry.hpp>
#include <neogfx/gfx/texture.hpp>

namespace neogfx
{
	struct vertex
	{
		vec3 coordinates;
		vec2 textureCoordinates;
	};
	typedef std::vector<vertex> vertex_list;
	typedef boost::optional<vertex_list> optional_vertex_list;
	typedef vertex_list::size_type vertex_index;
	typedef std::shared_ptr<vertex_list> vertex_list_pointer;

	struct face
	{
		std::array<vertex_index, 3> vertices;
		texture_index texture;
	};
	typedef std::vector<face> face_list;
	typedef boost::optional<face_list> optional_face_list;
	typedef std::shared_ptr<face_list> face_list_pointer;

	class i_mesh
	{
	public:
		struct no_textures : std::logic_error { no_textures() : std::logic_error("neogfx::i_mesh::no_textures") {} };
	public:
		virtual vertex_list_pointer vertices() const = 0;
		virtual texture_list_pointer textures() const = 0;
		virtual face_list_pointer faces() const = 0;
		virtual mat44 transformation_matrix() const = 0;
		virtual const vertex_list& transformed_vertices() const = 0;
	public:
		virtual void set_vertices(vertex_list_pointer aVertices) = 0;
		virtual void set_textures(texture_list_pointer aTextures) = 0;
		virtual void set_faces(face_list_pointer aFaces) = 0;
	};

	inline rect bounding_rect(const vertex_list& aVertices)
	{
		if (aVertices.empty())
			return rect{};
		point topLeft{ aVertices[0].coordinates.x, aVertices[0].coordinates.y };
		point bottomRight = topLeft;
		for (auto const& v : aVertices)
		{
			topLeft.x = std::min<coordinate>(topLeft.x, v.coordinates.x);
			topLeft.y = std::min<coordinate>(topLeft.y, v.coordinates.y);
			bottomRight.x = std::max<coordinate>(bottomRight.x, v.coordinates.x);
			bottomRight.y = std::max<coordinate>(bottomRight.y, v.coordinates.y);
		}
		return rect{ topLeft, bottomRight };
	}
}