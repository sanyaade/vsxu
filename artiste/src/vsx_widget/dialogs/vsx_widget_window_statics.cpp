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

#ifndef VSX_NO_CLIENT
#include <map>
#include <list>
#include "vsx_gl_global.h"
#include "vsx_command.h"
#include "vsx_texture_info.h"
#include "vsx_texture.h"
#include "vsx_math_3d.h"
#include "vsx_font.h"
#include "vsx_command.h"
#include "vsx_widget_base.h"
#include "lib/vsx_widget_lib.h"
#include "vsx_widget_window_statics.h"
#include "lib/vsx_widget_panel.h"
#include "lib/vsx_widget_base_edit.h"
//


dialog_query_string::dialog_query_string(vsx_string title_, vsx_string in_fields) {
  // buttons, always needed
  vsx_widget *button1 = add(new vsx_widget_button,".b1");
  vsx_widget *button2 = add(new vsx_widget_button,".b2");
  // now for the edit fields
  vsx_widget_window::init();
  if (in_fields != "") {
    vsx_avector<vsx_string> f_parts;
    vsx_string deli = "|";
    explode(in_fields, deli, f_parts);
    if (f_parts.size() == 0) f_parts.push_back("");
    set_size(vsx_vector(0.45f, 0.10f+(float)(f_parts.size())*0.04f));
    float yp = target_size.y - 0.04f-0.02f;
    edit1 = 0;
    for (unsigned int i = 0; i < f_parts.size(); ++i) {
      vsx_widget_base_edit *e = (vsx_widget_base_edit*)add(new vsx_widget_base_edit,"e");
      if (!edit1) edit1 = (vsx_widget*)e;
      edits.push_back((vsx_widget*)e);
      e->init();
      e->set_size(vsx_vector(size.x-0.04f, 0.02f));
      e->set_pos(vsx_vector(size.x-e->target_size.x*0.5-0.02,yp));
      e->set_font_size(0.02f);
      e->size_from_parent = true;
      e->single_row = true;
      e->set_string("");
      e->caret_goto_end();
      e->allowed_chars = " ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890!#�%&()=+?-_.,:/;";
      
      vsx_widget_2d_label *l = (vsx_widget_2d_label*)add(new vsx_widget_2d_label,"l");
      l->init();
      //l->set_size(vsx_vector(0.1));
      l->halign = a_left;
      l->set_pos(vsx_vector(e->pos.x-e->size.x*0.5,yp+0.02f));
      l->set_font_size(0.015f);
      l->title = f_parts[i];
      
      yp -= 0.04f;
    }
    
  }/* else {
    vsx_widget *label1 = add(new vsx_widget_2d_label,".l1");
    vsx_widget_base_edit *editor = add(new vsx_widget_base_edit,".e");
    vsx_widget_window::init();
    set_size(vsx_vector(0.3f, 0.14f));
    label1->pos.x = size.x/2;
    label1->pos.y = target_size.y-0.04;
    label1->title = hint;
    editor->set_pos(vsx_vector(label1->pos.x, label1->pos.y-0.02));
    editor->set_size(vsx_vector(0.25f, 0.02f));
    editor->size_from_parent = true;
    editor->single_row = true;
    editor->set_font_size(0.02f);
  }*/
//  edit1 = editor;
  title = title_;

  button1->title = "ok";
  button1->set_pos(vsx_vector(0.055,0.03));
  button1->commands.adds(4,"ok","ok","");

  button2->title = "cancel";
  button2->set_pos(vsx_vector(size.x-0.005-button2->size.x,0.03));
  button2->commands.adds(4,"cancel","cancel","cancel");
  init_run = true;
  coord_related_parent = false;
  visible = 0;
}

void dialog_query_string::set_allowed_chars(vsx_string ch) {
	((vsx_widget_base_edit*)edit1)->allowed_chars = ch;
}

void dialog_query_string::show() {
  a_focus = k_focus = edit1;
  visible = 1;
  set_pos(vsx_vector(0.5f-size.x/2, 0.5-size.y/2,0));
  parent->front(this);
}

void dialog_query_string::show(vsx_string value) {
  ((vsx_widget_base_edit*)edit1)->set_string(value);
  ((vsx_widget_base_edit*)edit1)->caret_goto_end();
  show();
}


void dialog_query_string::vsx_command_process_b(vsx_command_s *t) {
  if (t->cmd == "cancel") { 
    command_q_b.add(name+"_cancel","cancel");
    parent->vsx_command_queue_b(this);
    visible = 0; return;
  }
    vsx_string first_res;
      vsx_avector<vsx_string> res;
      for (unsigned long i = 0; i < edits.size(); ++i) {
        if (!first_res.size()) first_res = ((vsx_widget_base_edit*)(edits[i]))->get_string();
        else
        {
          //printf("res pushback\n");
          res.push_back( ((vsx_widget_base_edit*)(edits[i]))->get_string() );
          ((vsx_widget_base_edit*)(edits[i]))->set_string("");
        }
      }
      vsx_string i("|");
      vsx_string ress = implode(res, i);
      //printf("ress. %s\n",ress.c_str());
      vsx_string cmd = name+" "+first_res;
      if (ress.size()) {
        cmd += " "+base64_encode(ress); 
      }
      if (extra_value != "") cmd += " "+extra_value;
      command_q_b.add_raw(cmd);
//    } else {
//      command_q_b.add_raw(name+" "+((vsx_widget_base_edit*)edit1)->get_string());
//      parent->vsx_command_queue_b(this);
//      ((vsx_widget_base_edit*)edit1)->set_string("");
//    }
  //}
  visible = 0;
}




//******************************************************************************
//******************************************************************************
//******************************************************************************
//******************************************************************************
//******************************************************************************
//******************************************************************************
//******************************************************************************

dialog_messagebox::dialog_messagebox(vsx_string title_,vsx_string hint) {
  init_run = false;
  init();
  topmost = true;
  init_run = true;
  vsx_widget *button1 = add(new vsx_widget_button,"");
  std::vector<vsx_widget*> labels;
  std::vector <vsx_string> lines;
  vsx_string deli = "|";
  explode(hint,deli,lines);
  for (unsigned long i = 0; i < lines.size(); i++) {
    vsx_widget* b = add(new vsx_widget_2d_label,"");
    labels.push_back(b);
  }
  vsx_widget::init_children();

  size_t max_len = 0;
  for (unsigned long i = 0; i < lines.size(); ++i) {
    if (lines[i].size() > max_len) max_len = lines[i].size();
  }

  set_size(vsx_vector((float)max_len * 0.0094 , 0.10+(float)lines.size() * 0.017));
//  size.x = (float)max_len * 0.0094;

  float y = size.y*0.5+0.02;
  unsigned long i;
  for (i = 0; i < lines.size(); ++i) {
    labels[i]->set_pos(vsx_vector(size.x*0.5, y));
    labels[i]->target_size.y = labels[i]->size.y = 0.018;
    labels[i]->title = lines[i];
    y -= 0.017;
  }
  y -= 0.017;

  button1->title = "ok";
  button1->set_pos(vsx_vector(size.x*0.5,y));//(size.x-button1->size.x*0.5)*0.5,y));
  button1->commands.adds(4,"ok","ok","");

  title = title_;
  allow_resize_x = false;
  allow_resize_y = false;
  visible = 1;
  set_pos(vsx_vector(0.5-size.x/2,0.5-size.y/2));
  k_focus = this;
}

#endif
