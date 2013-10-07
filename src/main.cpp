#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include <harfbuzz/hb.h>
#include <harfbuzz/hb-ft.h>

#include <cwchar>
#include <iostream>
#include <cassert>
#include <stdio.h>


/*  See http://www.microsoft.com/typography/otspec/name.htm
    for a list of some possible platform-encoding pairs.
    We're interested in 0-3 aka 3-1 - UCS-2.
    Otherwise, fail. If a font has some unicode map, but lacks
    UCS-2 - it is a broken or irrelevant font. What exactly
    Freetype will select on face load (it promises most wide
    unicode, and if that will be slower that UCS-2 - left as
    an excercise to check. */
int force_ucs2_charmap(FT_Face ftf) {
    for(int i = 0; i < ftf->num_charmaps; i++)
        if ((  (ftf->charmaps[i]->platform_id == 0)
            && (ftf->charmaps[i]->encoding_id == 3))
           || ((ftf->charmaps[i]->platform_id == 3)
            && (ftf->charmaps[i]->encoding_id == 1)))
                return FT_Set_Charmap(ftf, ftf->charmaps[i]);
    return -1;
}

void ftfdump(FT_Face ftf) {
    for(int i=0; i<ftf->num_charmaps; i++) {
        printf("%d: %s %s %c%c%c%c plat=%hu id=%hu\n", i,
            ftf->family_name,
            ftf->style_name,
            ftf->charmaps[i]->encoding >>24,
           (ftf->charmaps[i]->encoding >>16 ) & 0xff,
           (ftf->charmaps[i]->encoding >>8) & 0xff,
           (ftf->charmaps[i]->encoding) & 0xff,
            ftf->charmaps[i]->platform_id,
            ftf->charmaps[i]->encoding_id
        );
    }
}

int main() 
{

  sf::RenderWindow window(sf::VideoMode(800, 600), "SFML window");
  // Limit the framerate to 60 frames per second (this step is optional)
  window.setFramerateLimit(60);

  const char *string = "مكتبة كتابة بالعربية!";
  /***** HARFBUZZ STUFF**********/
  FT_Library ft_library;
  assert(!FT_Init_FreeType(&ft_library));
  
  FT_Face ft_face;
  assert(!FT_New_Face(ft_library, "amiri-regular.ttf", 0, &ft_face));
  assert(!FT_Set_Char_Size(ft_face, 75*64, 75*64, 72, 72));
  ftfdump(ft_face);
  assert(force_ucs2_charmap(ft_face)!=-1);
  
  hb_font_t *hb_ft_font = hb_ft_font_create(ft_face, NULL);
  
  hb_buffer_t *buf = hb_buffer_create();
  hb_buffer_set_direction(buf, HB_DIRECTION_RTL); /* or LTR */
  hb_buffer_set_script(buf, HB_SCRIPT_ARABIC); /* see hb-unicode.h */
  hb_buffer_set_language(buf, hb_language_from_string("ar", strlen("ar")));
  
  hb_buffer_add_utf8(buf, string, strlen(string), 0, strlen(string));
  hb_shape(hb_ft_font, buf, NULL, 0);
  
  unsigned int         glyph_count;
  hb_glyph_info_t     *glyph_info = hb_buffer_get_glyph_infos(buf, &glyph_count);
  hb_glyph_position_t *glyph_pos = hb_buffer_get_glyph_positions(buf, &glyph_count);
  
  std::cout << "string length is: " << strlen(string) << std::endl;
  
  sf::Image output;
  output.create(800,600, sf::Color(255,255,255));
  
  int offset_x=0, offset_y=0;
  for(unsigned int i=0; i< glyph_count; i++) {
    assert(!FT_Load_Glyph(ft_face, glyph_info[i].codepoint, FT_LOAD_RENDER));
    FT_GlyphSlot glyph = ft_face->glyph;
    FT_Bitmap * bitmap = &glyph->bitmap;
    //printf("Glyph w: %u h: %u\n",bitmap->width, bitmap->rows );
    //printf("Offset x: %d y:%d, advance x:%d y:%d bbox %ld %ld\n",glyph_pos[i].x_offset/64, glyph_pos[i].y_offset/64, glyph_pos[i].x_advance/64 , glyph_pos[i].y_advance/64, ft_face->bbox.yMin, ft_face->bbox.yMax );
    //printf("bitmap_left %d bitmap_top %d\n", glyph->bitmap_left, glyph->bitmap_top);
    for(int y=0;y<bitmap->rows;y++)
      for(int x=0;x<bitmap->width;x++) {
	sf::Uint8 weight =  (sf::Uint8) bitmap->buffer[y*bitmap->width + x];
	int outx = 100 + offset_x/64 + glyph_pos[i].x_offset/64 + glyph->bitmap_left + x;
	int outy = 100 + offset_y/64 + glyph_pos[i].y_offset/64 - glyph->bitmap_top + y;
	sf::Uint8 newColor = 255-weight;
	sf::Uint8 originalColor= output.getPixel(outx,outy).r; //TODO: this won't work with colored background
	newColor = (weight + 255-originalColor) > 255 ? 255 : (weight + 255-originalColor); // Stitching and clamping
	newColor = 255-newColor;
	
	output.setPixel(outx, outy, sf::Color(newColor, newColor, newColor));
	
      }
    printf("ft.x:%lf ft.y:%lf hb.x:%f hb.y:%f\n", glyph->advance.x/64.0, glyph->advance.y/64.0, glyph_pos[i].x_advance/64.0, glyph_pos[i].y_advance/64.0);
    offset_x+=glyph_pos[i].x_advance;
    offset_y+=glyph_pos[i].y_advance;
  }
  output.saveToFile("output.png");
  return 0;
    //printf("glyph 0x%x\n", glyph_info[i].codepoint);
  
  /***** HARFBUZZ STUFF**********/
  
  //text stuff
  sf::Font mf;
  mf.loadFromFile("amiri-regular.ttf");
  sf::Text text;
  sf::String s(L"مرحباً");
  text.setString(s);
  text.setFont(mf);
  //text.setCharacterSize(30);
  text.setColor(sf::Color::Red);
  //text.setColor(sf::Color(255, 255, 255));

  // The main loop - ends as soon as the window is closed
  while (window.isOpen())
  {
    // Event processing
    sf::Event event;
    while (window.pollEvent(event))
    {
	// Request for closing the window
	if (event.type == sf::Event::Closed)
	    window.close();
    }
    // Clear the whole window before rendering a new frame
    window.clear(sf::Color(255,255,255));
    // Draw some graphical entities

    window.draw(text);
    // End the current frame and display its contents on screen
    window.display();
  }
}
