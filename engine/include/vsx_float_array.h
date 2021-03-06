/**
* Project: VSXu: Realtime visual programming language, music/audio visualizer, animation tool and much much more.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies Copyright (C) 2003-2011
* @see The GNU Public License (GPL)
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#ifndef VSX_FLOAT_ARRAY_H
#define VSX_FLOAT_ARRAY_H

class vsx_float_array {
public:
  vsx_array<float>* data;
  unsigned long timestamp;
  //vsx_float_array& operator=(vsx_float_array& t) {
    //for (int i = 0; i < t.data.size(); ++i) {
      //data.push_back(t.data[i]);
    //}
  //}
  vsx_float_array() {
    data = 0;
  }  
  //~vsx_float_array() {
    //delete data;
  //}  
};

class vsx_float3_array {
public:
  vsx_array<vsx_vector>* data;
  unsigned long timestamp;
  //vsx_float_array& operator=(vsx_float_array& t) {
    //for (int i = 0; i < t.data.size(); ++i) {
      //data.push_back(t.data[i]);
    //}
  //}
  vsx_float3_array() {
    data = 0;
  }  
  //~vsx_float_array() {
    //delete data;
  //}  
};  

class vsx_quaternion_array {
public:
  vsx_array<vsx_quaternion>* data;
  unsigned long timestamp;
  //vsx_float_array& operator=(vsx_float_array& t) {
    //for (int i = 0; i < t.data.size(); ++i) {
      //data.push_back(t.data[i]);
    //}
  //}
  vsx_quaternion_array() {
    data = 0;
  }
  //~vsx_float_array() {
    //delete data;
  //}
};

#endif
