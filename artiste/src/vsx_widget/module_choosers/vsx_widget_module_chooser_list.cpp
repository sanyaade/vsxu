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
#include <vector>
#include <map>
#include <list>
#include "vsx_param.h"
#include "vsx_module.h"
#include "vsx_gl_global.h"
#include "vsx_command.h"
#include "vsx_texture_info.h"
#include "vsx_texture.h"
#include "vsx_math_3d.h"
#include "vsx_font.h"
#include "vsx_command.h"
#include "vsx_widget_base.h"
#include "window/vsx_widget_window.h"
#include "lib/vsx_widget_lib.h"
#include "lib/vsx_widget_panel.h"
#include "lib/vsx_widget_base_edit.h"
#include <vsx_command_client_server.h>
#include "server/vsx_widget_server.h"
#include "server/vsx_widget_comp.h"
#include "dialogs/vsx_widget_window_statics.h"
#include "vsx_widget_module_chooser_list.h"
//
class vsx_widget_chooser_editor : public vsx_widget_base_editor {
  vsx_texture mtex_blob;
  vsx_widget* name_dialog;
  bool dragging;
  vsx_widget_coords drag_coords;
  int mod_i;
  vsx_string macro_name;
  vsx_vector drop_pos;
  vsx_widget* server;
  int draw_tooltip;
  vsx_string tooltip_text;
  vsx_vector tooltip_pos;

public:
  std::vector<int> i_rows_lookup;
  std::vector<vsx_module_info*> i_mod_info;


  vsx_widget_chooser_editor()
  {
    draw_tooltip = 0;
    dragging = false;
  }

  void set_server(vsx_widget* serv)
  {
    server = serv;
  }

  void extra_init()
  {
    dragging = false;
    editor->mirror_mouse_move_object = this;
    editor->mirror_mouse_move_passive_object = this;
    editor->mirror_mouse_up_object = this;
    editor->enable_syntax_highlighting = false;
    editor->font_size = 0.014;
    name_dialog = add(new dialog_query_string("name of component","Choose a unique name for your component"),"component_create_name");
    //((dialog_query_string*)name_dialog)->init();
    mtex_blob.load_png(skin_path+"interface_extras/connection_blob.png");
    set_render_type(VSX_WIDGET_RENDER_2D);
  }

  void event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords)
  {
    //printf("mouse move");
    mod_i = i_rows_lookup[editor->selected_line];
    if (mod_i != -1)
    {
      dragging = true;
      //printf("found module %s\n",i_mod_info[mod_i]->identifier.c_str());
      drag_coords = coords;
    }
  };

  void event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button)
  {
    if (dragging) dragging = false;
    //vsx_widget_base_edit::event_mouse_down(distance, coords, button);
  }

  virtual void event_mouse_move_passive(vsx_widget_distance distance,vsx_widget_coords coords)
  {
    int prev_caretx = editor->caretx;
    int prev_carety = editor->carety;
    int prev_selected_line = editor->selected_line;
    editor->event_mouse_down(distance, coords,0);  // ugly hack! LAMORZ ATTACK!
    if ((size_t)editor->selected_line < i_rows_lookup.size())
    {
      mod_i = i_rows_lookup[editor->selected_line];
    } else
      mod_i = -1;
    editor->caretx = prev_caretx;
    editor->carety = prev_carety;
    editor->selected_line = prev_selected_line;
    draw_tooltip = 0;
    if (mod_i != -1)
    {
      if (i_mod_info[mod_i]->description.size()>1)
      {
        tooltip_pos = coords.screen_global;
        tooltip_pos.x += 0.08;
        tooltip_text = i_mod_info[mod_i]->description;
        draw_tooltip = 1;
      }
      //printf("hovering over %s\n", i_mod_info[mod_i]->description.c_str() );
    }
  }

  virtual void i_draw()
  {
    vsx_widget_base_editor::i_draw();

  }

  virtual void draw_2d()// { if (render_type == VSX_WIDGET_RENDER_2D) { if (visible) i_draw(); } if (visible) draw_children_2d();}
  {
    i_draw();
    draw_children_2d();
    // draw the little box
    if (dragging && m_focus != editor) dragging = false;
    if (dragging)
    {
      mtex_blob.bind();
      glBlendFunc(GL_SRC_ALPHA, GL_ONE);
      float l_asp = screen_x/screen_y;
      //printf("screen aspect: %f\n",screen_aspect);
      glColor4f(1,1,1,1);
      draw_box_tex_c(drag_coords.screen_global, 0.03/l_asp, 0.03);
      mtex_blob._bind();
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    if (draw_tooltip && m_o_focus == editor && !dragging) {
      myf.color.a = 0.0f;
      myf.mode_2d = true;
      vsx_vector sz = myf.get_size(tooltip_text, 0.025f);
      //sz = sz-tooltip_pos;
      glColor4f(0.0f,0.0f,0.0f,0.6f);
      draw_box(vsx_vector(tooltip_pos.x,tooltip_pos.y+0.025*1.05), sz.x, -sz.y);
      glColor4f(1.0f,1.0f,1.0f,0.6f);
      myf.color.r = 1.0f;
      myf.color.a = 1.0f;
      tooltip_pos.z = 0;
      //printf("z: %f ",tooltip_pos.z);
      myf.print(tooltip_pos, tooltip_text, 0.022f);

    }
  }
  void event_mouse_up(vsx_widget_distance distance,vsx_widget_coords coords,int button)
  {
    if (dragging)
    {
      vsx_widget_distance l_distance;
      vsx_widget* tt = root->find_component(coords,l_distance);
      if (tt)
      {
        bool macro = false;
        if (tt->widget_type == VSX_WIDGET_TYPE_COMPONENT) if (((vsx_widget_component*)tt)->component_type == "macro") macro = true;
        if (macro) macro_name = tt->name+"."; else macro_name = "";
        if (tt->widget_type == VSX_WIDGET_TYPE_SERVER || macro) {
          drop_pos = l_distance.center;
          // split the identifier into the name
          vsx_avector<vsx_string> parts;
          vsx_string deli = ";";
          explode(i_mod_info[mod_i]->identifier, deli, parts);
          vsx_string module_name = parts[parts.size()-1];
          if (ctrl)
          ((dialog_query_string*)name_dialog)->show(((vsx_widget_server*)server)->get_unique_name(module_name));
          else
          {
            command_q_b.add_raw("component_create_name "+((vsx_widget_server*)server)->get_unique_name(module_name));
            vsx_command_queue_b(this);
          }
        }
      }
    }
  }

void vsx_command_process_b(vsx_command_s *t) {
  if (t->cmd == "cancel" || t->cmd == "component_create_name_cancel") {
    visible = 1;
    m_focus = this;
    k_focus = this;
    a_focus = this;
    dragging = false;
  } else
  if (t->cmd == "component_create_name") {
    visible = 1;
    m_focus = this;
    k_focus = this;
    a_focus = this;
    dragging = false;
    //printf("ident: %s\n",treedraw->selected->node->node->module_info->identifier.c_str());
    // check wich type it is
    if (i_mod_info[mod_i]->component_class == "macro") {
      //printf("macro in browser\n");
      // everything else will be contained in this macro, so modify the macro_name var
      vsx_string local_macro_name = t->cmd_data;
      // first create the macro
      command_q_b.add_raw("macro_create "+i_mod_info[mod_i]->identifier+" "+macro_name+local_macro_name+" "+f2s(drop_pos.x)+" "+f2s(drop_pos.y));
      // in here, send all the commands contained in the macro to the server..
      // this is terrible yes, but it will hopefully work :)
      server->vsx_command_queue_b(this);

    } else
    {
      // syntax:
      //  component_create math_logic;oscillator_dlux macro1.my_oscillator 0.013 0.204
      command_q_b.add_raw("component_create "+i_mod_info[mod_i]->identifier+" "+macro_name+t->cmd_data+" "+f2s(drop_pos.x)+" "+f2s(drop_pos.y));
      server->vsx_command_queue_b(this);
    }
  }
}

};


vsx_module_chooser_list::vsx_module_chooser_list() {
  // buttons, always needed
  //vsx_widget *button1 = add(new vsx_widget_button,".b1");
  //vsx_widget *button2 = add(new vsx_widget_button,".b2");
  // now for the edit fields
  vsx_widget_window::init();
  allow_resize_x = allow_resize_y = true;
  set_size(vsx_vector(0.15f, 0.7f));
  //float yp = target_size.y - 0.04f;
  vsx_widget_chooser_editor *e = (vsx_widget_chooser_editor*)add(new vsx_widget_chooser_editor,"e");
  e->init();
  e->set_render_type(VSX_WIDGET_RENDER_2D);
  e->coord_type = VSX_WIDGET_COORD_CORNER;
  coord_related_parent = false;
  //e->set_size(vsx_vector(size.x-0.04f, 0.12f));
  //e->set_pos(vsx_vector(size.x-e->target_size.x*0.5-0.02,yp-0.12f));
  e->set_pos(vsx_vector(size.x/2,size.y/2));
  e->editor->set_font_size(0.016f);
  e->size_from_parent = true;
  e->editor->editing_enabled = false;
  e->editor->selected_line_highlight = true;
  //e->editor->single_row = false;
  e->set_pos(vsx_vector(size.x/2,size.y/2));
  e->pos_from_parent = true;

  /*e->set_string(
      "+ renderers\n"
      " + basic\n"
      "   colored_rectangle\n"
      "   textured_rectangle\n"
      "+ renderers2\n"
      " + basic2\n"
      "   colored_rectangle2\n"
      "   textured_rectangle2\n"
      "+ renderers3\n"
      " + basic3\n"
      "   colored_rectangle3\n"
      "   textured_rectangle3\n"
      "+ renderers\n"
      " + basic\n"
      "   colored_rectangle\n"
      "   textured_rectangle\n"
      "+ renderers2\n"
      " + basic2\n"
      "   colored_rectangle2\n"
      "   textured_rectangle2\n"
      "+ renderers3\n"
      " + basic3\n"
      "   colored_rectangle3\n"
      "   textured_rectangle3\n"
      "+ renderers\n"
      " + basic\n"
      "   colored_rectangle\n"
      "   textured_rectangle\n"
      "+ renderers2\n"
      " + basic2\n"
      "   colored_rectangle2\n"
      "   textured_rectangle2\n"
      "+ renderers3\n"
      " + basic3\n"
      "   colored_rectangle3\n"
      "   textured_rectangle3\n"
      "+ renderers\n"
      " + basic\n"
      "   colored_rectangle\n"
      "   textured_rectangle\n"
      "+ renderers2\n"
      " + basic2\n"
      "   colored_rectangle2\n"
      "   textured_rectangle2\n"
      "+ renderers3\n"
      " + basic3\n"
      "   colored_rectangle3\n"
      "   textured_rectangle3\n"
      "+ renderers\n"
      " + basic\n"
      "   colored_rectangle\n"
      "   textured_rectangle\n"
      "+ renderers2\n"
      " + basic2\n"
      "   colored_rectangle2\n"
      "   textured_rectangle2\n"
      "+ renderers3\n"
      " + basic3\n"
      "   colored_rectangle3\n"
      "   textured_rectangle3\n"
      );*/
  //e->caret_goto_end();
  e->extra_init();
  edit = (vsx_widget*)e;
  e->extra_init();
  vsx_widget_base_edit *s = (vsx_widget_base_edit*)add(new vsx_widget_base_edit,"e");
  s->init();
  s->set_font_size(0.02f);
  s->size_from_parent = true;
  s->single_row = true;
  s->set_string("");
  s->caret_goto_end();
  s->allowed_chars = " ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890!#�%&()=+?-_.,:/;";
  s->mirror_keystrokes_object = this;
  search = (vsx_widget*)s;

  title = "Module List";

  init_run = true;
  visible = 0;
}

void vsx_module_chooser_list::set_server(vsx_widget* serv)
{
  ((vsx_widget_chooser_editor*)edit)->set_server(serv);
}

bool vsx_module_chooser_list::event_key_down(signed long key, bool alt, bool ctrl, bool shift)
{
  vsx_string filter = ((vsx_widget_base_edit*)search)->get_string();
  ((vsx_widget_base_editor*)edit)->editor->set_filter_string( filter );
  return true;
}

void vsx_module_chooser_list::show() {
  a_focus = k_focus = edit;
  visible = 1;
  set_pos(vsx_vector(0.0f, 0.0f/*0.5-size.y*0.75f*/,0));
}

void vsx_module_chooser_list::show(vsx_string value) {
  ((vsx_widget_base_editor*)edit)->set_string(value);
  ((vsx_widget_base_editor*)edit)->editor->caret_goto_end();
  show();
}

void vsx_module_chooser_list::i_draw()
{
  vsx_widget_window::i_draw();
  edit->set_pos(vsx_vector(size.x/2,size.y/2-font_size+dragborder*0.5f));
  edit->set_size(vsx_vector(size.x-dragborder*2,size.y-font_size*2-dragborder*2));
  search->set_size(vsx_vector(size.x-dragborder*2, 0.02f));
  search->set_pos(vsx_vector(size.x/2,size.y-0.04f));
}

void vsx_module_chooser_list::vsx_command_process_b(vsx_command_s *t) {
  if (t->cmd == "cancel") {
    command_q_b.add(name+"_cancel","cancel");
    parent->vsx_command_queue_b(this);
    visible = 0; return;
  }
/*    vsx_string first_res;
      vsx_avector<vsx_string> res;
      for (unsigned long i = 0; i < edits.size(); ++i) {
        if (!first_res.size()) first_res = ((vsx_widget_base_edit*)(edits[i]))->get_string();
        else
        {
          printf("res pushback\n");
          res.push_back( ((vsx_widget_base_edit*)(edits[i]))->get_string() );
          ((vsx_widget_base_edit*)(edits[i]))->set_string("");
        }
      }
      vsx_string i("|");
      vsx_string ress = implode(res, i);
      printf("ress. %s\n",ress.c_str());
      vsx_string cmd = name+" "+first_res;
      if (ress.size()) {
        cmd += " "+base64_encode(ress);
      }
      command_q_b.add_raw(cmd);*/
//    } else {
//      command_q_b.add_raw(name+" "+((vsx_widget_base_edit*)edit1)->get_string());
//      parent->vsx_command_queue_b(this);
//      ((vsx_widget_base_edit*)edit1)->set_string("");
//    }
  //}
  visible = 0;
}

void vsx_module_chooser_list::add_item(vsx_string name,vsx_module_info* m_info)
{
  i_rows.push_back(name);
  ((vsx_widget_chooser_editor*)edit)->i_mod_info.push_back(m_info);
}

void vsx_module_chooser_list::build_tree()
{
  std::vector<vsx_string> p_stack;
  vsx_string result;
  int module_id = 0;
  unsigned long i;
  unsigned long j;
  for (i = 0; i < i_rows.size(); i++) {
    std::vector<vsx_string> parts;
    vsx_string deli = ";";
    //printf("i_rows[i]: %s     %d\n", i_rows[i].c_str(),p_stack.size());
    explode(i_rows[i], deli, parts);
    for (j = 0; j < parts.size()-1; j++)
    {
      bool change = false;
      if (j == parts.size()-2 && p_stack.size() >= j+2)
      {
        while (p_stack.size() != j+1) p_stack.pop_back();
      }
      if (p_stack.size() >= j+1)
      {
        // stack has something, check if it's the same as we got
        if (p_stack[j] != parts[j])
        {
          // new category, trim down stack
          while (p_stack.size() != j) p_stack.pop_back();
          p_stack.push_back(parts[j]);
          change = true;
        }
      } else
      {
        // we know now that stack doesn't have this, just add and go on
        p_stack.push_back(parts[j]);
        change = true;
      }
      if (!change) continue;
      // add whitespaces
      for (unsigned long k = 0; k < (p_stack.size()-1) * 4; k++) result += " ";
      // add string
      result += "+ ";
      result += (parts[j]+"\n");
      ((vsx_widget_chooser_editor*)edit)->i_rows_lookup.push_back(-1);
    }
    for (unsigned long k = 0; k < (p_stack.size()) * 4; k++) result += " ";
    // add string
    result += (parts[parts.size()-1]+"\n");
    ((vsx_widget_chooser_editor*)edit)->i_rows_lookup.push_back(module_id);
    module_id++;
  }
  i_rows.clear();
//	printf("RESULT\nRESULT\nRESULT\nRESULT\n\n%s",result.c_str());
  ((vsx_widget_chooser_editor*)edit)->set_string(result);
  ((vsx_widget_chooser_editor*)edit)->editor->fold_all();
}


#endif
