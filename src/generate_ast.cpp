#include "common.hpp"
#include <iostream>
#include <iterator>
#include <string>
#include <string_view>
#include <vector>
#include <fstream>
#include <format>
#include <algorithm>

std::string to_lower(const std::string &str)
{
  std::string lstr;
  lstr.reserve(str.size());
  std::transform(str.begin(), str.end(), std::back_inserter(lstr), [](auto c) {
      return std::tolower(c);
  });
  return lstr;
}

std::string trim(const std::string &str)
{
  std::string_view sv(str);
  while (sv.begin() != sv.end() && std::isspace(*(sv.begin())))
  {
    sv.remove_prefix(1);
  }
  while (sv.rbegin() != sv.rend() && std::isspace(*(sv.rbegin())))
  {
    sv.remove_suffix(1);
  }
  return std::string(sv);
}

struct FileException : LoxException
{
  explicit FileException(const std::string &what = "") : LoxException(what)
  {
  }
};

struct FileWriter
{
  FileWriter(const std::string &path, std::ios_base::openmode mode)
    : writer{path, mode}, writer_iterator(writer)
  {
    if (!writer.is_open())
    {
      throw FileException(std::format("Failed to create file {}\n", path));
    }
  }
  ~FileWriter()
  {
    writer.close();
  }

  template<typename... Args>
  void write(const std::format_string<Args...>& fmt, Args&&... _args)
  {
     std::format_to(writer_iterator, fmt, std::forward<Args>(_args)...);
  }
  template<typename... Args>
  void write_line(const std::format_string<Args...>& fmt, Args&&... _args)
  {
     std::format_to(writer_iterator, fmt, std::forward<Args>(_args)...);
     new_line();
  }
  void new_line()
  {
    *writer_iterator = '\n';
    ++writer_iterator;
  }

private:
  std::ofstream writer;
  std::ostream_iterator<char> writer_iterator;
};

void generate_header(const std::string &output_dir, const std::string &base_name,
    const std::vector<std::string> &types)
{
  std::string file_name = std::format("{}/{}.hpp", output_dir, ::to_lower(base_name));
  FileWriter writer {file_name, std::ios::out};

  // Add header guard and needed headers
  writer.write_line("#pragma once");
  writer.new_line();
  writer.write_line("#include \"lexer.hpp\"");
  writer.write_line("#include <memory>");
  writer.write_line("#include <any>");
  writer.new_line();

  // Forward declare bases classes
  for (const auto&type : types)
  {
    auto class_name = trim(type.substr(0, type.find_first_of(':')));
    writer.write_line("struct {};", class_name);
  }
  writer.new_line();

  // Define Visitor class
  writer.write_line("struct {}Visitor", base_name);
  writer.write_line("{{");
  for (const auto&type : types)
  {
    auto class_name = trim(type.substr(0, type.find_first_of(':')));
    writer.write_line("  virtual std::any visit_{}({} &expr) = 0;", ::to_lower(class_name), class_name);
  }
  writer.write_line("}};");
  writer.new_line();

  // Define the base class
  writer.write_line("struct {}", base_name);
  writer.write_line("{{");
  writer.write_line("  virtual std::any accept({}Visitor &visitor) = 0;", base_name);
  writer.write_line("}};");
  writer.new_line();

  // Define derived classes
  for(const auto& type : types)
  {
    auto class_name = trim(type.substr(0, type.find_first_of(':')));
    writer.write_line("struct {} : {}", class_name, base_name);
    writer.write_line("{{");
    std::string_view sv(type);

    // Find the first ':' and remove everything up to that point if it exists
    auto colon_pos = sv.find_first_of(':');
    if (colon_pos != std::string_view::npos) {
      sv.remove_prefix(colon_pos + 1);
    }

    std::vector<std::string> fields;

    // extract the name of derived classes
    do {
      // Find the next ',' or the end of the string view
      auto comma_pos = sv.find_first_of(',');

      // Extract and trim the substring up to the comma (or to the end if no comma found)
      std::string current_type = trim(std::string(sv.substr(0, comma_pos)));
      fields.emplace_back(current_type);

      // Remove the processed part of the string view
      if (comma_pos != std::string_view::npos) {
        sv.remove_prefix(comma_pos + 1);
      } else {
        sv.remove_prefix(sv.size()); // Reached the end
      }

    } while (!sv.empty());

    // define derived constructor
    writer.write("  explicit {} (", class_name);
    for(int i{0}; i < fields.size() - 1; ++i)
    {
      writer.write("{}, ", fields[i]);
    }
    writer.write("{})", fields[fields.size() - 1]);
    writer.new_line();
    writer.write("    : ");
    for(int i{0}; i < fields.size() - 1; ++i)
    {
      auto pos = fields[i].find_last_of(' ');
      auto name = fields[i].substr(pos + 1, fields[i].size() - pos);
      writer.write("{}{{std::move({})}}, ", name, name);
    }
    auto pos = fields[fields.size() - 1].find_last_of(' ');
    auto name = fields[fields.size() - 1].substr(pos + 1, fields[fields.size() - 1].size() - pos);
    writer.write("{}{{std::move({})}}", name, name);
    writer.new_line();
    writer.write_line("  {{");
    writer.write_line("  }}");
    writer.new_line();
    
    // define accept method
    writer.write_line("  std::any accept({}Visitor &visitor) override", base_name);
    writer.write_line("  {{");
    writer.write_line("    return visitor.visit_{}(*this);", ::to_lower(class_name));
    writer.write_line("  }}");
    writer.new_line();

    // finally define fields
    for(const auto &field : fields)
    {
      writer.write_line("  {};", field);
    }

    writer.write_line("}};");
    writer.new_line();
  }
}

void define_ast(const std::string &output_dir, const std::string &base_name,
                const std::vector<std::string> &types)
{
  generate_header(output_dir, base_name, types);
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: generate_ast <output_dir>\n";
    return EX_USAGE;
  }
  std::string output_dir{argv[1]};
  try 
  {
    define_ast(output_dir, "Expr",
        {"Binary   : std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right",
        "Grouping : std::unique_ptr<Expr> expression", "Literal  : Token value",
        "Unary    : Token op, std::unique_ptr<Expr> right"});
  }
  catch (const LoxException &exception)
  {
    std::cerr << exception.what() << "\n";
  }
  return 0;
}
