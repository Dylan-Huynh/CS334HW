// generated by Fast Light User Interface Designer (fluid) version 1.0304

#include "gui.h"

void GUI::cb_DBG_i(Fl_Button*, void*) {
    DBG_cb();
}
void GUI::cb_DBG(Fl_Button* o, void* v) {
    ((GUI*)(o->parent()->user_data()))->cb_DBG_i(o, v);
}

void GUI::cb_SM2_i(Fl_Button*, void*) {
    SM2_cb();
}
void GUI::cb_SM2(Fl_Button* o, void* v) {
    ((GUI*)(o->parent()->user_data()))->cb_SM2_i(o, v);
}

void GUI::cb_SM3_i(Fl_Button*, void*) {
    SM3_cb();
}
void GUI::cb_SM3(Fl_Button* o, void* v) {
    ((GUI*)(o->parent()->user_data()))->cb_SM3_i(o, v);
}

void GUI::cb_Left_i(Fl_Repeat_Button*, void*) {
    Left_cb();
}
void GUI::cb_Left(Fl_Repeat_Button* o, void* v) {
    ((GUI*)(o->parent()->user_data()))->cb_Left_i(o, v);
}

void GUI::cb_Up_i(Fl_Repeat_Button*, void*) {
    Up_cb();
}
void GUI::cb_Up(Fl_Repeat_Button* r, void* v) {
    ((GUI*)(r->parent()->user_data()))->cb_Up_i(r, v);
}

void GUI::cb_Down_i(Fl_Repeat_Button*, void*) {
    Down_cb();
}
void GUI::cb_Down(Fl_Repeat_Button* s, void* v) {
    ((GUI*)(s->parent()->user_data()))->cb_Down_i(s, v);
}

void GUI::cb_Right_i(Fl_Repeat_Button*, void*) {
    Right_cb();
}
void GUI::cb_Right(Fl_Repeat_Button* t, void* v) {
    ((GUI*)(t->parent()->user_data()))->cb_Right_i(t, v);
}

void GUI::cb_Light_Left_i(Fl_Repeat_Button*, void*) {
    Light_Left_cb();
}
void GUI::cb_Light_Left(Fl_Repeat_Button* o, void* v) {
    ((GUI*)(o->parent()->user_data()))->cb_Light_Left_i(o, v);
}

void GUI::cb_Light_Up_i(Fl_Repeat_Button*, void*) {
    Light_Up_cb();
}
void GUI::cb_Light_Up(Fl_Repeat_Button* r, void* v) {
    ((GUI*)(r->parent()->user_data()))->cb_Light_Up_i(r, v);
}

void GUI::cb_Light_Down_i(Fl_Repeat_Button*, void*) {
    Light_Down_cb();
}
void GUI::cb_Light_Down(Fl_Repeat_Button* s, void* v) {
    ((GUI*)(s->parent()->user_data()))->cb_Light_Down_i(s, v);
}

void GUI::cb_Light_Right_i(Fl_Repeat_Button*, void*) {
    Light_Right_cb();
}
void GUI::cb_Light_Right(Fl_Repeat_Button* t, void* v) {
    ((GUI*)(t->parent()->user_data()))->cb_Light_Right_i(t, v);
}

void GUI::cb_NewButton_i(Fl_Return_Button*, void*) {
    NewButton_cb();
}
void GUI::cb_NewButton(Fl_Return_Button* o, void* v) {
    ((GUI*)(o->parent()->user_data()))->cb_NewButton_i(o, v);
}

void GUI::cb_Ambient_i(Fl_Button*, void*) {
    Ambient_cb();
}
void GUI::cb_Ambient(Fl_Button* o, void* v) {
    ((GUI*)(o->parent()->user_data()))->cb_Ambient_i(o, v);
}

void GUI::cb_AmbientDown_i(Fl_Button*, void*) {
    AmbientDown_cb();
}
void GUI::cb_AmbientDown(Fl_Button* o, void* v) {
    ((GUI*)(o->parent()->user_data()))->cb_AmbientDown_i(o, v);
}

void GUI::cb_WhiteUp_i(Fl_Button*, void*) {
    WhiteUp_cb();
}
void GUI::cb_WhiteUp(Fl_Button* o, void* v) {
    ((GUI*)(o->parent()->user_data()))->cb_WhiteUp_i(o, v);
}

void GUI::cb_WhiteDown_i(Fl_Button*, void*) {
    WhiteDown_cb();
}
void GUI::cb_WhiteDown(Fl_Button* o, void* v) {
    ((GUI*)(o->parent()->user_data()))->cb_WhiteDown_i(o, v);
}



#include "scene.h"

GUI::GUI() {
    // For a ton of these I just made new buttons and tested the dimensions to make sure they made sense
    { uiw = new Fl_Double_Window(800, 397, "GUI");
    uiw->user_data((void*)(this));
    { Fl_Button* o = new Fl_Button(15, 15, 95, 40, "DBG");
    o->selection_color(FL_DARK_RED);
    o->callback((Fl_Callback*)cb_DBG);
    } // Fl_Button* o
    { Fl_Button* p = new Fl_Button(15, 55, 95, 40, "SM2");
    p->selection_color(FL_DARK_RED);
    p->callback((Fl_Callback*)cb_SM2);
    } // Fl_Button* o
    { Fl_Button* r = new Fl_Button(15, 95, 95, 40, "SM3");
    r->selection_color(FL_DARK_RED);
    r->callback((Fl_Callback*)cb_SM3);
    } // Fl_Button* o
    { Fl_Return_Button* o = new Fl_Return_Button(30, 200, 110, 100, "NewButton");
    o->callback((Fl_Callback*)cb_NewButton);
    } // Fl_Return_Button* o
    { Fl_Repeat_Button* o = new Fl_Repeat_Button(300, 250, 100, 50, "Left");
    o->callback((Fl_Callback*)cb_Left);
    }
    { Fl_Repeat_Button* r = new Fl_Repeat_Button(250, 200, 100, 50, "Up");
    r->callback((Fl_Callback*)cb_Up);
    }
    { Fl_Repeat_Button* s = new Fl_Repeat_Button(250, 300, 100, 50, "Down");
    s->callback((Fl_Callback*)cb_Down);
    }
    { Fl_Repeat_Button* t = new Fl_Repeat_Button(200, 250, 100, 50, "Right");
    t->callback((Fl_Callback*)cb_Right);
    }
    { Fl_Repeat_Button* o = new Fl_Repeat_Button(600, 250, 100, 50, "Light_Left");
    o->callback((Fl_Callback*)cb_Light_Left);
    }
    { Fl_Repeat_Button* r = new Fl_Repeat_Button(550, 200, 100, 50, "Light_Up");
    r->callback((Fl_Callback*)cb_Light_Up);
    }
    { Fl_Repeat_Button* s = new Fl_Repeat_Button(550, 300, 100, 50, "Light_Down");
    s->callback((Fl_Callback*)cb_Light_Down);
    }
    { Fl_Repeat_Button* t = new Fl_Repeat_Button(500, 250, 100, 50, "Light_Right");
    t->callback((Fl_Callback*)cb_Light_Right);
    }
    { Fl_Button* p = new Fl_Button(200, 000, 100, 50, "AmbientUp");
    p->callback((Fl_Callback*)cb_Ambient);
    } // Fl_Button* p
    { Fl_Button* q = new Fl_Button(300, 000, 100, 50, "AmbientDown");
    q->callback((Fl_Callback*)cb_AmbientDown);
    } // Fl_Button* q
    { Fl_Button* p = new Fl_Button(200, 100, 100, 50, "WhiteUp");
    p->callback((Fl_Callback*)cb_WhiteUp);
    } // Fl_Button* p
    { Fl_Button* q = new Fl_Button(300, 100, 100, 50, "WhiteDown");
    q->callback((Fl_Callback*)cb_WhiteDown);
    } // Fl_Button* q
    uiw->end();
    } // Fl_Double_Window* uiw
}

int main(int argc, char** argv) {
    scene = new Scene;
    return Fl::run();
}

void GUI::show() {
    uiw->show();
}

void GUI::DBG_cb() {
    scene->DBG();
}

void GUI::SM2_cb() {
    scene->SM2();
}
void GUI::SM3_cb() {
    scene->SM3();
}

void GUI::NewButton_cb() {
    scene->NewButton();
}

void GUI::Left_cb() {
    scene->Left();
}

void GUI::Up_cb() {
    scene->Up();
}

void GUI::Down_cb() {
    scene->Down();
}

void GUI::Right_cb() {
    scene->Right();
}

void GUI::Light_Left_cb() {
    scene->Light_Left();
}

void GUI::Light_Up_cb() {
    scene->Light_Up();
}

void GUI::Light_Down_cb() {
    scene->Light_Down();
}

void GUI::Light_Right_cb() {
    scene->Light_Right();
}

void GUI::Ambient_cb() {
    scene->Ambient();
}

void GUI::AmbientDown_cb() {
    scene->AmbientDown();
}

void GUI::WhiteUp_cb() {
    scene->WhiteUp();
}

void GUI::WhiteDown_cb() {
    scene->WhiteDown();
}