#include "GUI.h"

nanogui::Theme *textbox_theme;
nanogui::Theme *button_theme;

void default_theme_init(nanogui::Theme *t) {
    t->mWindowCornerRadius = 0;
    t->mWindowDropShadowSize = 0;
    t->mButtonCornerRadius = 3;
    t->mTabBorderWidth = 30;
    t->mWindowHeaderGradientTop = nanogui::Color(200, 200, 200, 0);
    t->mWindowHeaderGradientBot = nanogui::Color(200, 200, 200, 0);
    t->mTransparent = nanogui::Color(255, 255, 255, 0);
    t->mWindowFillUnfocused = nanogui::Color(180, 180, 180, 150);
    t->mWindowFillFocused = nanogui::Color(180, 180, 180, 150);
    t->mWindowHeaderSepTop = nanogui::Color(200, 200, 200, 0);
    t->mWindowHeaderSepBot = nanogui::Color(200, 200, 200, 0);
    t->mWindowTitleUnfocused = nanogui::Color(0, 0);
    t->mWindowTitleFocused = nanogui::Color(0, 0);
    t->mWindowPopup = nanogui::Color(180, 180, 180, 150);
    t->mFontBold = 3;
    t->mTextColorShadow = nanogui::Color(0, 0, 0, 0);
    t->mDropShadow = nanogui::Color(0, 0, 0, 0);
    t->mWindowHeaderHeight = 8;
    t->mStandardFontSize = 12;
    t->mButtonFontSize = 12;
    t->mBorderDark = nanogui::Color(255, 255, 255, 0);
    t->mBorderLight = nanogui::Color(255, 255, 255, 0);
    t->mButtonGradientTopPushed = nanogui::Color(180, 180, 180, 255);
    t->mButtonGradientBotPushed = nanogui::Color(180, 180, 180, 255);
    t->mButtonGradientTopFocused = nanogui::Color(180, 180, 180, 255);
    t->mButtonGradientBotFocused = nanogui::Color(180, 180, 180, 255);
    t->mButtonGradientTopUnfocused = nanogui::Color(255, 255, 255, 255);
    t->mButtonGradientBotUnfocused = nanogui::Color(255, 255, 255, 255);
    t->mTextColor = nanogui::Color(0, 255);
}

void textbox_theme_init(nanogui::Theme *textbox_theme){
    textbox_theme->mTextColor = nanogui::Color(0, 255);
    textbox_theme->mButtonFontSize = 12;
    textbox_theme->mWindowCornerRadius = 10;
    textbox_theme->mButtonCornerRadius = 10;
    textbox_theme->mBorderDark = nanogui::Color(0, 255);
    textbox_theme->mBorderLight = nanogui::Color(0, 255);
    textbox_theme->mTextColor = nanogui::Color(0, 255);
    textbox_theme->mDisabledTextColor = nanogui::Color(0, 255);
}

void button_theme_init(nanogui::Theme *button_theme) {
    button_theme->mTextColor = nanogui::Color(255, 255);
    button_theme->mButtonFontSize = 12;
    // button_theme->mDropShadow = nanogui::Color(0, 0, 0, 0);
    button_theme->mBorderDark = nanogui::Color(255, 255, 255, 0);
    button_theme->mBorderLight = nanogui::Color(255, 255, 255, 0);
    button_theme->mButtonCornerRadius = 3;
}


MeshList::MeshList(nanogui::Widget *parent, std::vector<std::string> item_names) 
    : nanogui::PopupButton(parent, item_names[0]), selectedIndex(0){
    // pb = new nanogui::PopupButton(parent, item_names[selectedIndex]);
    this->setTextColor(nanogui::Color(0, 255));
    this->setFixedSize(Eigen::Vector2i(120,20));
    nanogui::Popup *popup = this->popup();
    popup->setLayout(new nanogui::GridLayout(nanogui::Orientation::Horizontal, 1,nanogui::Alignment::Middle, 10, 0));

    toolbox = new nanogui::Widget(popup);
    toolbox->setLayout(new nanogui::GridLayout(nanogui::Orientation::Horizontal, 1,nanogui::Alignment::Maximum, 0, 0));
    names = item_names;
    cumulative_rel = Eigen::Vector2f(0,0);

    for (int i=0; i<item_names.size(); i++) {
        addItem(item_names[i], i);
    }

    nanogui::Widget *tmp = new nanogui::Widget(popup);
    tmp->setLayout(new nanogui::GridLayout(nanogui::Orientation::Horizontal, 1,nanogui::Alignment::Maximum, 0, 0));
    open_mesh = new nanogui::Button(tmp, "Open");
    open_mesh->setBackgroundColor(nanogui::Color(74, 74, 74, 255));
    open_mesh->setTheme(button_theme);
    open_mesh->setFixedSize(Eigen::Vector2i(120,20));
}

nanogui::Button * MeshList::addItem(const std::string item_name, int i) {
    nanogui::Button *b = new nanogui::Button(toolbox, item_name);
    b->setBackgroundColor(nanogui::Color(255, 255, 255, 255));
    b->setFixedSize(Eigen::Vector2i(120,20));
    b->setCallback([this, item_name, i]{
        this->selectedIndex = i;
        this->popup()->setVisible(false);
        this->setPushed(false);
        this->setCaption(item_name);
        cumulative_rel(1) = selectedIndex * 6.0;
    });
    items.push_back(b);
    return b;
}

bool MeshList::scrollEvent(const Eigen::Vector2i &p, const Eigen::Vector2f &rel) {
    if (cumulative_rel.y() + rel.y() > 0 && cumulative_rel.y() + rel.y() < names.size() * 6.0) {
        cumulative_rel = cumulative_rel + rel;
    }
    // printf("cumulative_rel.y: %f\n", cumulative_rel.y());
    int next_selectedIndex = std::min((int)(cumulative_rel.y()/6.0), (int)(names.size()-1));
    if (next_selectedIndex != selectedIndex) {
        selectedIndex = next_selectedIndex;
    }
    this->setCaption(names[selectedIndex]);
    return Widget::scrollEvent(p, rel);
}

Row::Row(nanogui::Label *left, nanogui::Widget *right) {
    lab = left;
    w = right;
}

Row::Row(nanogui::Label *left, ColorController *right) {
    lab = left;
    w = right->pb;
}

void Row::setVisible(bool visible) {
    this->lab->setVisible(visible);
    this->w->setVisible(visible);
}

ColorController::ColorController(nanogui::Window *nanoguiWindow, nanogui::Color c) {
    // nanogui::Label* lab = new nanogui::Label(nanoguiWindow, "Diffuse", "sans");
    nanogui::Label* lab;
    pb = new nanogui::PopupButton(nanoguiWindow, "");
    pb->setTextColor(nanogui::Color(0, 255));
    pb->setFixedSize(Eigen::Vector2i(120,20));
    pb->setBackgroundColor(c);
    pb->setTextColor(c.contrastingColor());
    nanogui::Popup *popup = pb->popup();
    popup->setLayout(new nanogui::GridLayout(nanogui::Orientation::Horizontal, 1,nanogui::Alignment::Middle, 10, 0));

    nanogui::Widget *tools;
    tools = new nanogui::Widget(popup);
    tools->setLayout(new nanogui::GridLayout(nanogui::Orientation::Horizontal, 1,nanogui::Alignment::Maximum, 5, 0));
    cw = new nanogui::ColorWheel(tools, c);

    tools = new nanogui::Widget(popup);
    nanogui::GridLayout *layout = new nanogui::GridLayout(nanogui::Orientation::Horizontal, 2,nanogui::Alignment::Middle, 0, 0);
    layout->setColAlignment({nanogui::Alignment::Maximum, nanogui::Alignment::Fill});
    tools->setLayout(layout);

    lab = new nanogui::Label(tools, "Red", "sans");
    tb_r = new nanogui::IntBox<int>(tools);
    tb_r->setEditable(true);
    tb_r->setFixedSize(Eigen::Vector2i(40, 20));
    tb_r->setValue((int) (c.r()* 255.0f));
    // tb_r->setTheme(textbox_theme);
    tb_r->setFontSize(12);
    tb_r->setFormat("[0-9]*");
    tb_r->setMinMaxValues(0, 255);

    lab = new nanogui::Label(tools, "Green", "sans");
    tb_g = new nanogui::IntBox<int>(tools);
    tb_g->setEditable(true);
    tb_g->setFixedSize(Eigen::Vector2i(40, 20));
    tb_g->setValue((int) (c.g()* 255.0f));
    // tb_g->setDefaultValue("0.0");
    // tb_g->setTheme(textbox_theme);
    tb_g->setFontSize(12);
    tb_g->setFormat("[0-9]*");
    tb_g->setMinMaxValues(0, 255);

    lab = new nanogui::Label(tools, "Blue", "sans");
    tb_b = new nanogui::IntBox<int>(tools);
    tb_b->setEditable(true);
    tb_b->setFixedSize(Eigen::Vector2i(40, 20));
    tb_b->setValue((int) (c.b()* 255.0f));
    // tb_b->setTheme(textbox_theme);
    tb_b->setFontSize(12);
    tb_b->setFormat("[0-9]*");
    tb_b->setMinMaxValues(0, 255);
}

nanogui::Color ColorController::intbox_callback(int channel, int n) {
    nanogui::Color c = cw->color();
    int clipped_n = 255;
    if (n < 255) clipped_n = n;

    if (channel == 0) c = nanogui::Color(clipped_n, (int) (c.g()* 255.0f), (int) (c.b()* 255.0f), 255);
    else if (channel == 1) c = nanogui::Color((int) (c.r()* 255.0f), clipped_n, (int) (c.b()* 255.0f), 255);
    else c = nanogui::Color((int) (c.r()* 255.0f), (int) (c.g()* 255.0f), clipped_n, 255);

    cw->setColor(c);
    pb->setBackgroundColor(c);
    return c;
}

void ColorController::colorwheel_callback(nanogui::Color c) {
    pb->setBackgroundColor(c);
    pb->setTextColor(c.contrastingColor());
    int red = (int) (c.r() * 255.0f);
    tb_r->setValue(red);
    int green = (int) (c.g() * 255.0f);
    tb_g->setValue(green);
    int blue = (int) (c.b() * 255.0f);
    tb_b->setValue(blue);
}


void GUI::combo_init(nanogui::ComboBox *combo_box) {
    combo_box->setSelectedIndex(0);
    combo_box->setTextColor(nanogui::Color(0, 255));
    combo_box->setFixedSize(Eigen::Vector2i(120,20));
    combo_box->setCallback([](int a) {});
    // p = combo_box->popup();
    const std::vector<nanogui::Widget *> &children3 = combo_box->popup()->children();
    for (int i=0; i<combo_box->items().size(); i++) {
        // ((nanogui::Button *) children3[i])->setBackgroundColor(nanogui::Color(255, 255, 255, 255));
        ((nanogui::Button *) children3[i])->setTextColor(nanogui::Color(0, 255));
        ((nanogui::Button *) children3[i])->setFixedSize(Eigen::Vector2i(120,20));
    }
    nanogui::Popup* p = combo_box->popup();
    p->setLayout(new nanogui::BoxLayout(nanogui::Orientation::Vertical, nanogui::Alignment::Middle, 10, 0));
    p->setAnchorHeight(26);
}

void GUI::nanogui_init(GLFWwindow* window) {
	screen = new nanogui::Screen();
    screen->initialize(window, true);
    nanoguiWindow = new nanogui::Window(screen, "Control Panel");
    nanoguiWindow->setPosition(Eigen::Vector2i(20,20));

    // theme
    nanogui::Theme *t = new nanogui::Theme(screen->nvgContext());
    default_theme_init(t);
    screen->setTheme(t);

    textbox_theme = new nanogui::Theme(screen->nvgContext());
    textbox_theme_init(textbox_theme);

    button_theme = new nanogui::Theme(screen->nvgContext());
    button_theme_init(button_theme);

    //layout
    nanogui::GridLayout *layout = new nanogui::GridLayout(nanogui::Orientation::Horizontal, 2,nanogui::Alignment::Middle, 15, 0);
    layout->setColAlignment({nanogui::Alignment::Maximum, nanogui::Alignment::Fill});
    nanoguiWindow->setLayout(layout);

    nanogui::Label* lab;

    lab = new nanogui::Label(nanoguiWindow, "Render Options", "sans");
    render_mode = new nanogui::ComboBox(nanoguiWindow, {"Blinn-Phong", "PBR Basic"});
    combo_init(render_mode);

    lab = new nanogui::Label(nanoguiWindow, "Tex Or Color", "sans");
    texOrColor = new nanogui::ComboBox(nanoguiWindow, {"Colorwheel", "Texture"});
    combo_init(texOrColor);

    lab = new nanogui::Label(nanoguiWindow, "Assets", "sans");
    // ml = new MeshList(nanoguiWindow, {"buddha.obj", "dragon.obj", "bunny3.obj", "nefertiti.obj"});
    ml = new MeshList(nanoguiWindow, {"bunny3.obj", "sample3.obj", "sample1.obj", "sample2.obj"});

    lab = new nanogui::Label(nanoguiWindow, "Rotate Camera", "sans");
    b1 = new nanogui::Button(nanoguiWindow, "Pause/Resume");
    b1->setBackgroundColor(nanogui::Color(74, 74, 74, 255));
    b1->setTheme(button_theme);
    b1->setFixedSize(Eigen::Vector2i(120,20));

    lab = new nanogui::Label(nanoguiWindow, "Shineness", "sans");
    tb = new nanogui::IntBox<int>(nanoguiWindow);
    tb->setEditable(true);
    tb->setFixedSize(Eigen::Vector2i(120, 20));
    tb->setValue(100);
    tb->setTheme(textbox_theme);
    tb->setFontSize(12);
    tb->setFormat("[0-9]*");

    tb->setSpinnable(true);
    tb->setMinValue(1);
    tb->setValueIncrement(1);

    shineness = new Row(lab, tb);
    shineness->setVisible(true);

    lab = new nanogui::Label(nanoguiWindow, "Diffuse", "sans");
    cc = new ColorController(nanoguiWindow, nanogui::Color(110,100,160, 255));
    lab = new nanogui::Label(nanoguiWindow, "Specular", "sans");
    cc2 = new ColorController(nanoguiWindow, nanogui::Color(100,100,100, 255));
    lab = new nanogui::Label(nanoguiWindow, "Ambient", "sans");
    cc3 = new ColorController(nanoguiWindow, nanogui::Color(40,40,40, 255));
    ambient = new Row(lab, cc3);
    ambient->setVisible(true);

    lab = new nanogui::Label(nanoguiWindow, "Glossiness", "sans");
    nanogui::IntBox<int>* tb2 = new nanogui::IntBox<int>(nanoguiWindow);
    tb2->setEditable(true);
    tb2->setFixedSize(Eigen::Vector2i(120, 20));
    tb2->setValue(60);
    tb2->setTheme(textbox_theme);
    tb2->setFontSize(12);
    tb2->setFormat("[0-9]*");

    tb2->setSpinnable(true);
    tb2->setMinMaxValues(1, 99);
    tb2->setValueIncrement(1);
    
    glossiness = new Row(lab, tb2);
    glossiness->setVisible(false);


    lab = new nanogui::Label(nanoguiWindow, "Metallic", "sans");
    nanogui::IntBox<int>* tb3 = new nanogui::IntBox<int>(nanoguiWindow);
    tb3->setEditable(true);
    tb3->setFixedSize(Eigen::Vector2i(120, 20));
    tb3->setValue(50);
    tb3->setTheme(textbox_theme);
    tb3->setFontSize(12);
    tb3->setFormat("[0-9]*");

    tb3->setSpinnable(true);
    tb3->setMinMaxValues(1, 99);
    tb3->setValueIncrement(1);

    metallic = new Row(lab, tb3);
    metallic->setVisible(false);

    screen->setVisible(true);
    screen->performLayout();
}

bool GUI::in_window(Eigen::Vector2i c) {
    Eigen::Vector2i p = nanoguiWindow->position();
    int w = nanoguiWindow->width();
    int h = nanoguiWindow->height();

    if ((c(0) <= p(0) + w) && 
        (c(0) >= p(0)) && 
        (c(1) <= p(1) + h) && 
        (c(1) >= p(1))) 
    {
        return true;
    }
    return false;
}
