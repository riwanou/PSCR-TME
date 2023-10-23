#pragma once

#include "Barrier.h"
#include "Rayon.h"
#include "Scene.h"
#include "Vec3D.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <random>

namespace pr {

class Job {
public:
  virtual void run() = 0;
  virtual ~Job(){};
};

// Job concret : exemple

class PixelJob : public Job {
  Barrier &b;
  const Scene::screen_t &screen;
  const Scene &scene;
  std::vector<Vec3D> &lights;
  Color *pixels;
  int x;
  int y;

  // return the index of the closest object in the scene that intersects "ray"
  // or -1 if the ray does not intersect any object.
  int findClosestInter(const Scene &scene, const Rayon &ray) {
    auto minz = std::numeric_limits<float>::max();
    int targetSphere = -1;
    int index = 0;
    for (const auto &obj : scene) {
      // rend la distance de l'objet a la camera
      auto zinter = obj.intersects(ray);
      // si intersection plus proche  ?
      if (zinter < minz) {
        minz = zinter;
        targetSphere = index;
      }
      index++;
    }
    return targetSphere;
  }

  // Calcule l'angle d'incidence du rayon à la sphere, cumule l'éclairage des
  // lumières En déduit la couleur d'un pixel de l'écran.
  Color computeColor(const Sphere &obj, const Rayon &ray, const Vec3D &camera,
                     std::vector<Vec3D> &lights) {
    Color finalcolor = obj.getColor();

    // calcul du rayon et de sa normale a la sphere
    // on prend le vecteur de la camera vers le point de l'ecran (dest -
    // origine) on le normalise a la longueur 1, on multiplie par la distance à
    // l'intersection
    Vec3D rayInter = (ray.dest - ray.ori).normalize() * obj.intersects(ray);
    // le point d'intersection
    Vec3D intersection = rayInter + camera;
    // la normale a la sphere au point d'intersection donne l'angle pour la
    // lumiere
    Vec3D normal = obj.getNormale(intersection);
    // le niveau d'eclairage total contribue par les lumieres 0 sombre 1 total
    // lumiere
    double dt = 0;
    // modifier par l'eclairage la couleur
    for (const auto &light : lights) {
      // le vecteur de la lumiere au point d'intersection
      Vec3D tolight = (light - intersection);
      // si on est du bon cote de la sphere, i.e. le rayon n'intersecte pas
      // avant de l'autre cote
      if (obj.intersects(Rayon(light, intersection)) >=
          tolight.length() - 0.05) {        //  epsilon 0.05 for double issues
        dt += tolight.normalize() & normal; // l'angle (scalaire) donne la
                                            // puissance de la lumiere reflechie
      }
    }
    // eclairage total
    finalcolor = finalcolor * dt +
                 finalcolor * 0.2; // *0.2 = lumiere speculaire ambiante

    return finalcolor;
  }

  void calculate() {
    // le point de l'ecran par lequel passe ce rayon
    auto &screenPoint = screen[y][x];
    // le rayon a inspecter
    Rayon ray(scene.getCameraPos(), screenPoint);
    int targetSphere = findClosestInter(scene, ray);

    if (targetSphere == -1) {
      // keep background color
      return;
    } else {
      const Sphere &obj = *(scene.begin() + targetSphere);
      // pixel prend la couleur de l'objet
      Color finalcolor = computeColor(obj, ray, scene.getCameraPos(), lights);
      // le point de l'image (pixel) dont on vient de calculer la couleur
      Color &pixel = pixels[y * scene.getHeight() + x];
      // mettre a jour la couleur du pixel dans l'image finale.
      pixel = finalcolor;
    }
  }

public:
  PixelJob(Barrier &b, const Scene::screen_t &screen, const Scene &scene,
           std::vector<Vec3D> &lights, Color *pixels, int x, int y)
      : b(b), screen(screen), scene(scene), lights(lights), pixels(pixels),
        x(x), y(y) {}

  void run() {
    calculate();
    b.done();
  }
};

} // namespace pr
