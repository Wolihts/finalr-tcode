#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "vector3.hpp"
#include "triangle.hpp"
//same as the rastizer, need for the triangle struct and vector3 struct, also need the stb_image_write.h for writing the image, the rest of the code is in this file since its not that big and it makes it easier to read and test
#include "file_util.h"
#include <cmath>
#include <vector>
#include <string>
using namespace std;

//helper func for splitting a line by a char
vector<string> split_line(string line,char c)
{
  vector<string> out;
  string cur = "";

  for(int i = 0; i < (int)line.size(); i++)
  {
    if(line[i] == c)
    {
      if(cur != "")
      {
        out.push_back(cur);
        cur = "";
      }
    }
    else
    {
      cur += line[i];
    }
  }

  if(cur != "")
    out.push_back(cur);

  return out;
}

Vector3 g_s(Vector3 pos, Vector3 n, Vector3 uv, Vector3 l)
{
  Vector3 ld = l;
  float df = ld.normalized().dot(n.normalized());
  df = max(0.f,df);
  float it = .2f + df;
  float r = .05f * it;
  float g = .85f * it;
  float b = .25f * it;
  return {r,g,b};
}

Vector3 s_o(Vector3 pos, Vector3 n, Vector3 uv, Vector3 l)
{
  Vector3 ld = l;
  float df = ld.normalized().dot(n.normalized());
  df = max(0.f,df);
  float it = .25f + df;
  float r = .55f * it;
  float g = .32f * it;
  float b = .16f * it;
  return {r,g,b};
}

Vector3 s_r(Vector3 pos, Vector3 n, Vector3 uv, Vector3 l)
{
  Vector3 ld = l;
  float df = ld.normalized().dot(n.normalized());
  df = max(0.f,df);
  float it = .25f + df;
  float r = .8f * it;
  float g = .05f * it;
  float b = .05f * it;
  return {r,g,b};
}

Vector3 s_y(Vector3 pos, Vector3 n, Vector3 uv, Vector3 l)
{
  Vector3 ld = l;
  float df = ld.normalized().dot(n.normalized());
  df = max(0.f,df);
  float it = .25f + df;
  float r = .75f * it;
  float g = .68f * it;
  float b = .25f * it;
  return {r,g,b};
}

Vector3 s_p(Vector3 pos, Vector3 n, Vector3 uv, Vector3 l)
{
  Vector3 ld = l;
  float df = ld.normalized().dot(n.normalized());
  df = max(0.f,df);
  float it = .25f + df;
  float r = .45f * it;
  float g = .15f * it;
  float b = .85f * it;
  return {r,g,b};
}

Vector3 check_s(Vector3 pos, Vector3 n, Vector3 uv, Vector3 l)
{
  Vector3 ld = l;
  float df = ld.normalized().dot(n.normalized());
  df = max(0.f,df);
  float it = .25f + df;
  float a = floor(uv.x * 10.f);
  float b = floor(uv.y * 10.f);
  int ck = (int)(a + b) % 2;
  Vector3 c1 = {.05f,.05f,.05f};
  Vector3 c2 = {.95f,.95f,.95f};
  Vector3 col = ck == 0 ? c1 : c2;

  return {col.x * it,col.y * it,col.z * it};
}

bool rhit(vector<Triangle> tris, Vector3 ro, Vector3 rd, Triangle &tri_out, float &near_t, Vector3 &hit_pos, Vector3 &hit_n, Vector3 &hit_uv)
{
  for(auto &tri : tris)
  {
    Vector3 a = tri.p0;
    Vector3 b = tri.p1;
    Vector3 c = tri.p2;
    Vector3 ab = b.sub(a);
    Vector3 ac = c.sub(a);
    Vector3 n = ab.cross(ac);
    float bot = n.dot(rd);
    if(abs(bot) < .00001f)
      continue;

    float d = -n.dot(a);
    float t = (-d - ro.dot(n)) / bot;

    if(t > near_t || t < 0)
      continue;

    Vector3 p = ro.add(rd.times(t));
    float area = n.length() / 2;
    Vector3 pa = p.sub(a);
    Vector3 pb = p.sub(b);
    Vector3 pc = p.sub(c);
    float aa = pb.cross(pc).length() / 2;
    float bb = pa.cross(pc).length() / 2;
    float cc = pa.cross(pb).length() / 2;
    float wa = aa / area;
    float wb = bb / area;
    float wc = cc / area;
    float e = .00001f;
    bool inside = abs(1 - (wa + wb + wc)) < e;

    if(!inside)
      continue;
    near_t = t;
    hit_pos = p;
    hit_n = tri.n0.times(wa).add(tri.n1.times(wb)).add(tri.n2.times(wc));
    hit_uv = tri.uv0.times(wa).add(tri.uv1.times(wb)).add(tri.uv2.times(wc));
    tri_out = tri;
  }
  return near_t != INFINITY;
}
Vector3 rcolor(vector<Triangle> tris, vector<Vector3> lights, Vector3 ro, Vector3 rd, float &near_t, Vector3 bg, int left)
{
  if(left == 0)
    return {0,0,0};
  Vector3 hp;
  Vector3 hn;
  Vector3 hu;
  Triangle hit_tri;
  Vector3 pix = {0,0,0};

  if(rhit(tris,ro,rd,hit_tri,near_t,hp,hn,hu))
  {
    for(auto &l : lights)
    {
      Triangle sh_tri;
      float sh_t = INFINITY;
      Vector3 sh_p;
      Vector3 sh_n;
      Vector3 sh_u;

      if(!rhit(tris,hp.add(l.times(.0001f)),l,sh_tri,sh_t,sh_p,sh_n,sh_u))
        pix = pix.add(hit_tri.shader(hp,hn,hu,l));
    }

    if(hit_tri.reflectivity != 0)
    {
      Vector3 refl = rd.sub(hn.normalized().times(2 * hn.normalized().dot(rd)));
      float nxt_t = INFINITY;
      pix = pix.times(1 - hit_tri.reflectivity).add(rcolor(tris,lights,hp.add(refl.times(.0001f)),refl,nxt_t,bg,left - 1).times(hit_tri.reflectivity));
    }
  }
  return near_t != INFINITY ? pix : bg;
}

void load_obj(string name, vector<Triangle> &tris, Vector3 shift, float sc)
{
  vector<Vector3> ps;
  vector<Vector3> ns;
  vector<Vector3> us;

  char* obj = load_file(name.c_str());

  if(obj == NULL)
    return;

  vector<string> lines = split(obj);
  free(obj);

  for(int line_i = 0; line_i < (int)lines.size(); line_i++)
  {
    string line = lines[line_i];

    if(line.size() < 2)
      continue;

    vector<string> p = split_line(line,' ');
    if(p.size() == 0)
      continue;
    if(p[0] == "v")
    {
      if(p.size() < 4)
        continue;

      Vector3 v;
      v.x = stof(p[1]) * sc + shift.x;
      v.y = stof(p[2]) * sc + shift.y;
      v.z = stof(p[3]) * sc + shift.z;
      ps.push_back(v);
    }
    else if(p[0] == "vt")
    {
      if(p.size() < 3)
        continue;

      Vector3 u;
      u.x = stof(p[1]);
      u.y = stof(p[2]);
      u.z = 0.f;
      us.push_back(u);
    }
    else if(p[0] == "vn")
    {
      if(p.size() < 4)
        continue;

      Vector3 n;
      n.x = stof(p[1]);
      n.y = stof(p[2]);
      n.z = stof(p[3]);
      ns.push_back(n.normalized());
    }
    else if(p[0] == "f")
    {
      vector<int> pi;
      vector<int> ui;
      vector<int> ni;

      for(int i = 1; i < (int)p.size(); i++)
      {
        vector<string> nums = split_line(p[i],'/');
        int a = -1;
        int b = -1;
        int c = -1;
        if(nums.size() > 0 && nums[0] != "")
          a = stoi(nums[0]) - 1;

        if(nums.size() > 1 && nums[1] != "")
          b = stoi(nums[1]) - 1;

        if(nums.size() > 2 && nums[2] != "")
          c = stoi(nums[2]) - 1;

        pi.push_back(a);
        ui.push_back(b);
        ni.push_back(c);
      }

      for(int i = 1; i + 1 < (int)pi.size(); i++)
      {
        Vector3 a = ps[pi[0]];
        Vector3 b = ps[pi[i]];
        Vector3 c = ps[pi[i + 1]];
        Vector3 fn = b.sub(a).cross(c.sub(a)).normalized();
        Vector3 na = fn;
        Vector3 nb = fn;
        Vector3 nc = fn;

        if(ni[0] >= 0 && ni[0] < (int)ns.size())
          na = ns[ni[0]];
        if(ni[i] >= 0 && ni[i] < (int)ns.size())
          nb = ns[ni[i]];
        if(ni[i + 1] >= 0 && ni[i + 1] < (int)ns.size())
          nc = ns[ni[i + 1]];

        Vector3 ua = {a.x * .2f,a.y * .2f,0};
        Vector3 ub = {b.x * .2f,b.y * .2f,0};
        Vector3 uc = {c.x * .2f,c.y * .2f,0};
        if(ui[0] >= 0 && ui[0] < (int)us.size())
          ua = us[ui[0]];
        if(ui[i] >= 0 && ui[i] < (int)us.size())
          ub = us[ui[i]];
        if(ui[i + 1] >= 0 && ui[i + 1] < (int)us.size())
          uc = us[ui[i + 1]];

        tris.push_back({a,b,c,na,nb,nc,ua,ub,uc,check_s,.25f});
      }
    }
  }
}

int main()
{
  vector<Vector3> lights;
  lights.push_back({1,2,.5});
  lights.push_back({-.5f,1.2f,.2f});
  Vector3 cam = {0,0,0};
  const float s = 8;
  const float z = -18;

  vector<Triangle> tris;

  Vector3 uv_a = {0.f,0.f,0.f};
  Vector3 uv_b = {1.f,0.f,0.f};
  Vector3 uv_c = {0.f,1.f,0.f};
  Vector3 uv_d = {1.f,1.f,0.f};

  Vector3 v000 = {-s,-s,-s + z};
  Vector3 v001 = {-s,-s,s + z};
  Vector3 v010 = {-s,+s,-s + z};
  Vector3 v011 = {-s,+s,s + z};
  Vector3 v100 = {+s,-s,-s + z};
  Vector3 v101 = {+s,-s,s + z};
  Vector3 v110 = {+s,+s,-s + z};
  Vector3 v111 = {+s,+s,s + z};

  Vector3 nb = v001.sub(v000).cross(v101.sub(v000)).normalized();
  Vector3 nl = v010.sub(v000).cross(v001.sub(v000)).normalized();
  Vector3 nr = v101.sub(v100).cross(v110.sub(v100)).normalized();
  Vector3 nk = v000.sub(v010).cross(v110.sub(v010)).normalized();

  tris.push_back({v000,v001,v101,nb,nb,nb,uv_a,uv_c,uv_d,s_o,.3f});
  tris.push_back({v000,v101,v100,nb,nb,nb,uv_a,uv_d,uv_b,s_o,.3f});

  tris.push_back({v000,v010,v001,nl,nl,nl,uv_a,uv_b,uv_c,s_p,.2f});
  tris.push_back({v011,v001,v010,nl,nl,nl,uv_d,uv_c,uv_b,s_p,.2f});
  tris.push_back({v100,v101,v110,nr,nr,nr,uv_a,uv_c,uv_b,s_y,.2f});
  tris.push_back({v111,v110,v101,nr,nr,nr,uv_d,uv_b,uv_c,s_y,.2f});
  tris.push_back({v010,v000,v110,nk,nk,nk,uv_a,uv_c,uv_b,s_r,.15f});
  tris.push_back({v100,v110,v000,nk,nk,nk,uv_d,uv_b,uv_c,s_r,.15f});

  //using an obj file from blender, this took me a lot of time to test since it was big(about 5000 lines)
  load_obj("Untitled.obj",tris,{0.f,-1.5f,-11.f},1.5f);

  Vector3 bg = {.03f,.03f,.05f};
  const int w = 640;
  const int h = 640;
  unsigned char img[w * h * 3];
  for(float y = 0; y < h; y++)
  {
    for(float x = 0; x < w; x++)
    {
      float near_t = INFINITY;
      Vector3 pix = {0,0,0};
      float sx = x / w * 2 - 1;
      float sy = -(y / h * 2 - 1);
      Vector3 rd = (Vector3{sx,sy,-1}.sub(cam)).normalized();
      Vector3 col = rcolor(tris,lights,cam,rd,near_t,bg,5);
      int k = (y * w + x) * 3;
      img[k] = (unsigned char)(255 * min(1.f,col.x));
      img[k + 1] = (unsigned char)(255 * min(1.f,col.y));
      img[k + 2] = (unsigned char)(255 * min(1.f,col.z));
    }
  }
  stbi_write_png("image.png",w,h,3,img,w * 3);
  return 0;
}