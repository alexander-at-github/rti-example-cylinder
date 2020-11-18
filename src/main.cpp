#include <cmath>
#include <cstdlib>
#include <iostream>
#include <omp.h>

#include <rti/device.hpp>

#include "custom_reflection.hpp"
#include "particle.hpp"
//#include "io/create_plot.hpp"
#include "io/vtp_point_cloud_reader.hpp"
#include "io/vtp_writer.hpp"

int main(int argc, char** argv)
{
  //omp_set_num_threads(1);
  using numeric_type = float;

  auto input = io::vtp_point_cloud_reader<numeric_type>("../src/io/cylinder-25-rotated.vtp");
  auto points = input.get_points();
  auto normals = input.get_normals();
  auto radii = input.get_radii();

  auto rtidevice = rti::device<numeric_type, particle<numeric_type> > {};
  rtidevice.set_points(points);
  rtidevice.set_normals(normals);
  rtidevice.set_grid_spacing(radii);
  rtidevice.set_number_of_rays(1024 * 1024);
  rtidevice.set_x(rti::bound_condition::REFLECTIVE);
  rtidevice.set_y(rti::bound_condition::PERIODIC);
  //rtidevice.set_cosine_source(); // default
  rtidevice.set_power_cosine_source(2); // exponent
  //auto specular = rti::reflection::specular<numeric_type> {};
  //auto diffuse = rti::reflection::diffuse<numeric_type> {}; // default
  auto customreflection = custom_reflection<numeric_type> {0.5};
  rtidevice.set(customreflection);
  rtidevice.run();
  auto mcestimates = rtidevice.get_mc_estimates();
  auto hitcnts = rtidevice.get_hit_cnts();

  io::vtp_writer<numeric_type>::write(points, normals, radii, mcestimates, hitcnts, "result-file.vtp");
  //io::create_plot(points, mcestimates);

  exit(EXIT_SUCCESS);
}
