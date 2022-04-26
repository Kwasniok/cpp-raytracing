#include <array>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <tao/pegtl.hpp>
#include <tao/pegtl/contrib/parse_tree.hpp>
#include <tao/pegtl/contrib/parse_tree_to_dot.hpp>
#include <tao/pegtl/contrib/trace.hpp>

namespace pegtl = tao::pegtl;

// grammar

struct ws_char : pegtl::one<' ', '\t', '\n', '\r'> {};
struct ws : pegtl::plus<ws_char> {};
struct opt_ws : pegtl::opt<ws> {};

struct separator : pegtl::string<','> {};
struct quote : pegtl::one<'"'> {};

struct assign : pegtl::pad<pegtl::string<'='>, ws> {};

struct bracket_open : pegtl::string<'{'> {};
struct bracket_close : pegtl::string<'}'> {};

struct digits : pegtl::plus<pegtl::digit> {};
struct exp : pegtl::seq<pegtl::one<'e', 'E'>, pegtl::opt<pegtl::one<'-', '+'>>,
                        digits> {};
struct frac : pegtl::seq<pegtl::one<'.'>, digits> {};
struct real : pegtl::seq<pegtl::opt<pegtl::one<'-', '+'>>, digits,
                         pegtl::opt<frac>, pegtl::opt<exp>> {};

struct identifier : pegtl::star<pegtl::alnum> {};
struct quoted_identifier : pegtl::seq<quote, identifier, quote> {
    using content = identifier;
};

struct material_body
    : pegtl::seq<bracket_open,
                 pegtl::pad<pegtl::seq<pegtl::string<'i', 'd', 'e', 'n', 't',
                                                     'i', 'f', 'i', 'e', 'r'>,
                                       assign, identifier>,
                            ws>,
                 , bracket_close> {};
struct material : pegtl::seq<material_prefix, pegtl::pad<material_body>, ws> {};
struct material_list
    : pegtl::seq<bracket_open, pegtl::star<pegtl::pad<material, ws>, separator>,
                 opt_ws, bracket_close> {};

struct material_name : quoted_identifier {};

struct sphere_body
    : pegtl::seq<
          bracket_open,
          pegtl::pad<pegtl::seq<pegtl::string<'r', 'a', 'd', 'i', 'u', 's'>,
                                assign, real>,
                     ws>,
          bracket_close> {};
struct sphere_prefix : pegtl::string<'S', 'p', 'h', 'e', 'r', 'e'> {};
struct sphere : pegtl::seq<sphere_prefix, pegtl::pad<sphere_body, ws>> {};

struct geometry : pegtl::sor<sphere> {};

struct position : pegtl::seq<bracket_open, pegtl::pad<real, ws>, separator,
                             pegtl::pad<real, ws>, separator,
                             pegtl::pad<real, ws>, bracket_close> {};

struct object_body
    : pegtl::seq<
          bracket_open,
          pegtl::pad<
              pegtl::seq<pegtl::string<'p', 'o', 's', 'i', 't', 'i', 'o', 'n'>,
                         assign, position>,
              ws>,
          separator,
          pegtl::pad<
              pegtl::seq<pegtl::string<'g', 'e', 'o', 'm', 'e', 't', 'r', 'y'>,
                         assign, geometry>,
              ws>,
          separator,
          pegtl::pad<
              pegtl::seq<pegtl::string<'m', 'a', 't', 'e', 'r', 'i', 'a', 'l'>,
                         assign, material_name>,
              ws>,
          bracket_close> {};
struct object_prefix : pegtl::string<'O', 'b', 'j', 'e', 'c', 't'> {};
struct object : pegtl::seq<object_prefix, pegtl::pad<object_body, ws>> {};

struct object_list
    : pegtl::seq<bracket_open, pegtl::star<pegtl::pad<object, ws>, separator>,
                 opt_ws, bracket_close> {};

struct scene_body
    : pegtl::seq<
          bracket_open,
          pegtl::pad<pegtl::seq<pegtl::string<'m', 'a', 't', 'e', 'r', 'i', 'a',
                                              'l', 's'>,
                                assign, pegtl::seq<material_list>>,
                     ws>,
          separator pegtl::pad<
              pegtl::seq<pegtl::string<'o', 'b', 'j', 'e', 'c', 't', 's'>,
                         assign, pegtl::seq<object_list>>,
              ws>,
          bracket_close> {};
struct scene_prefix : pegtl::string<'S', 'c', 'e', 'n', 'e'> {};
struct scene : pegtl::seq<scene_prefix, pegtl::pad<scene_body, ws>> {};

struct file_prefix : pegtl::string<'R', 'A', 'Y'> {};
struct file : pegtl::seq<file_prefix, pegtl::pad<scene, ws>> {};

struct grammar : pegtl::must<file, pegtl::eof> {};

// selector (partial parse tree generation)

template <typename Rule>
using selector = tao::pegtl::parse_tree::selector<
    Rule, tao::pegtl::parse_tree::store_content::on<real, identifier>,
    tao::pegtl::parse_tree::remove_content::on<
        scene, object_list, object, position, geometry, sphere, material_name>>;

// data structure

template <typename T, std::size_t N>
std::ostream& operator<<(std::ostream& os, const std::array<T, N>& array) {
    os << '{';
    for (const auto& elem : array) {
        os << elem << ",";
    }
    os << '}';
    return os;
}
template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec) {
    for (const auto& elem : vec) {
        os << elem << ",";
    }
    return os;
}

using Real = double;
using Identifier = std::string;
using Vector = std::array<Real, 3>;

struct Geometry {
    virtual ~Geometry() = default;

    virtual std::ostream& write(std::ostream& os) const = 0;
};
std::ostream& operator<<(std::ostream& os, const Geometry& geometry) {
    geometry.write(os);
    return os;
}
struct Sphere : public Geometry {
    Real radius;

    Sphere(Real radius) : radius(radius) {}
    virtual ~Sphere() = default;

    virtual std::ostream& write(std::ostream& os) const override {
        os << "Sphere {"
           << "radius = " << radius << ","
           << "}";
        return os;
    };
};
struct Material {
    Identifier identifier;

    Material() : identifier("invalid") {} // TODO: remove
    Material(const char* identifier) : identifier(identifier) {}
    Material(Identifier&& identifier) : identifier(std::move(identifier)) {}

    virtual ~Material() = default;

    virtual std::ostream& write(std::ostream& os) const {
        os << "Material {"
           << "identifier = " << identifier << ","
           << "}";
        return os;
    };
};
std::ostream& operator<<(std::ostream& os, const Material& material) {
    material.write(os);
    return os;
}
struct Object {
    Vector position;
    std::unique_ptr<Geometry> geometry;
    std::shared_ptr<Material> material;
};
std::ostream& operator<<(std::ostream& os, const Object& object) {
    os << "Object {"
       << "position = " << object.position << ","
       << "geometry = " << *object.geometry << ","
       << "material = "
       << (object.material ? object.material->identifier : "nullptr") << ",";
    os << "}";
    return os;
}
struct Scene {
    std::vector<Object> objects;
    std::vector<std::shared_ptr<Material>> materials;
};
std::ostream& operator<<(std::ostream& os, const Scene& scene) {
    os << "Scene {"
       //<< "materials = " << scene.materials << ","
       << "objects = " << scene.objects << ","
       << "}";
    return os;
}

Real real_factory(const pegtl::parse_tree::node& node) {
    std::cerr << "Real << " << node.type << " (" << node.string() << ") "
              << std::endl;
    Real real = std::stod(node.string().c_str());
    return real;
}

Vector vector_factory(const pegtl::parse_tree::node& node) {
    std::cerr << "Vector << " << node.type << std::endl;
    Vector vector;
    for (std::size_t i = 0; i < node.children.size(); ++i) {
        vector[i] = real_factory(*node.children[i]);
    }
    return vector;
}

Identifier identifier_factory(const pegtl::parse_tree::node& node) {
    std::cerr << "Identifier << " << node.type << " (" << node.string() << ") "
              << std::endl;
    Identifier identifier = node.string();
    return identifier;
}

std::unique_ptr<Material>
material_factory(const pegtl::parse_tree::node& node) {
    std::cerr << "Material << " << node.type << std::endl;
    // TOOD: dummy
    return std::make_unique<Material>();
}

std::unique_ptr<Sphere> sphere_factory(const pegtl::parse_tree::node& node) {
    std::cerr << "Geometry << " << node.type << std::endl;
    Real radius = real_factory(*node.children[0]);
    return std::make_unique<Sphere>(radius);
}

std::unique_ptr<Geometry>
geometry_factory(const pegtl::parse_tree::node& node) {
    std::cerr << "Geometry << " << node.type;
    const std::string_view subtype = node.children[0]->type;
    std::cerr << " (" << subtype << ")" << std::endl;
    if (subtype == "sphere") {
        return sphere_factory(*node.children[0]);
    }
    return {};
}

std::shared_ptr<Material>
find_material(const std::vector<std::shared_ptr<Material>>& materials,
              Identifier&& identifier) {
    for (const auto& mat : materials) {
        if (mat && mat->identifier == identifier) {
            return mat;
        }
    }
    return {};
}

Object object_factory(const pegtl::parse_tree::node& node,
                      std::vector<std::shared_ptr<Material>>& materials) {
    std::cerr << "Object << " << node.type << std::endl;
    Vector position = vector_factory(*node.children[0]);
    std::unique_ptr<Geometry> geometry = geometry_factory(*node.children[1]);
    Identifier material_id = identifier_factory(*node.children[2]->children[0]);
    // TODO: dummy
    std::shared_ptr<Material> material =
        find_material(materials, std::move(material_id));
    Object object = {.position = position,
                     .geometry = std::move(geometry),
                     .material = std::move(material)};
    return object;
}

Scene scene_factory(const pegtl::parse_tree::node& node) {
    std::cerr << "Scene << " << node.type << std::endl;
    Scene scene;
    // {
    //     const pegtl::parse_tree::node& material_list = *(node.children[0]);
    //     for (const auto& mat : material_list.children) {
    //         Material material = material_factory(*mat);
    //         scene.materials.push_back(material);
    //     }
    // }
    {
        const pegtl::parse_tree::node& object_list = *(node.children[0]);
        for (const auto& obj : object_list.children) {
            Object object = object_factory(*obj, scene.materials);
            scene.objects.push_back(std::move(object));
        }
    }
    return scene;
};

int main() {

    std::string input = "RAY \n"
                        "Scene { \n"
                        "  objects = { \n"
                        "    Object { \n"
                        "      position={1,2,3}, \n"
                        "      geometry=Sphere{radius=1.0}, \n"
                        "      material=\"mat1\" \n"
                        "    }, \n"
                        "    Object { \n"
                        "      position={4,5,6}, \n"
                        "      geometry=Sphere{radius=7.0}, \n"
                        "      material=\"mat2\" \n"
                        "    }, \n"
                        "  } \n"
                        "}";

    {
        pegtl::memory_input in(input, "");
        try {
            const auto root = pegtl::parse_tree::parse<grammar, selector>(in);
            if (root) {
                // pipe with `| dot -Tsvg -o parse_tree.svg` to generate an svg
                // pegtl::parse_tree::print_dot(std::cout, *root);
                Scene scene = scene_factory(*(root->children[0]));
                std::cout << scene << std::endl;
            }
        } catch (const pegtl::parse_error& e) {
            const auto p = e.positions().front();
            std::cerr << e.what() << std::endl
                      << in.line_at(p) << '\n'
                      << std::setw(p.column) << '^' << std::endl;
        }
    }
    // {
    //     pegtl::memory_input in(input, "");
    //     pegtl::standard_trace<grammar>(in);
    // }
    // {
    //     pegtl::memory_input in(input, "");
    //     pegtl::parse<grammar>(in, state);
    //     for (const auto& o : state.objects)
    //         std::cout << o << std::endl;
    // }

    return 0;
}
