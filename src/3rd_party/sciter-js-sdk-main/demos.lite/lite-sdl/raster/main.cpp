#include "SDL3/SDL.h"
#include "SDL3/SDL_main.h"

// SCITER {
#include "sciter-x.h"
#include "sciter-x-behavior.h"
#include "sciter-x-dom.hpp"

UINT SC_CALLBACK handle_notification(LPSCITER_CALLBACK_NOTIFICATION pnm, LPVOID callbackParam);
void dispatch_event(SDL_Window* window, SDL_Renderer *renderer, SDL_Event& event);

// } SCITER


bool sciter_needs_drawing = false;

SDL_Surface* bitmap = nullptr;

void resize_bitmap(SDL_Window* window) {
  int w, h;
  if (SDL_GetWindowSize(window, &w, &h)) {
    if (bitmap)
      SDL_DestroySurface(bitmap);
#ifdef WINDOWS
    bitmap = SDL_CreateSurface(w, h, SDL_PIXELFORMAT_BGRA32);
#else
    bitmap = SDL_CreateSurface(w, h, SDL_PIXELFORMAT_RGBA32);
#endif    
    SL_SURFACE surface = {};
    surface.bitmap.pixels = bitmap->pixels;
    surface.bitmap.stride = bitmap->pitch;
    SciterProcX(window, SCITER_X_MSG_SIZE(w, h, surface));
  }
}

int main(int argc, char* argv[]) {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
    return 1;
  }

  SDL_Window* window = SDL_CreateWindow("Hello, SDL3!", 800, 600, SDL_WINDOW_RESIZABLE);
  if (!window) {
    SDL_Log("Could not create window: %s", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
  if (!renderer) {
    SDL_Log("Could not create renderer: %s", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  // Sciter: create engine instance associated with the window
  SciterProcX(window, SCITER_X_MSG_CREATE(SL_TARGET_BITMAP, NULL));
  // Sciter: set callback:
  SciterSetCallback(window, handle_notification, NULL);

  resize_bitmap(window);

  // Sciter: set device resoultion ( pixels per inch )
  float scale = SDL_GetWindowDisplayScale(window);
  SciterProcX(window, SCITER_X_MSG_RESOLUTION(UINT(96 * scale)));

  //Sciter: load document
  const char* html = "<html style='background:gold;font-size:20pt;font-weight:bold'><body>Hello <a href=#>SDL!</a></body></html>";
  SciterLoadHtml(window, (LPCBYTE)html, strlen(html),WSTR("about:blank"));

  //SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  //SDL_RenderClear(renderer);
  //SDL_RenderPresent(renderer);

  bool quit = false;

  // Event handler
  SDL_Event event;

  // Main loop
  while (!quit) {
    // Handle events
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        quit = true;
      }
      dispatch_event(window, renderer, event);
    }

    SciterProcX(window, SCITER_X_MSG_HEARTBIT(event.common.timestamp * 1000000));

    if (sciter_needs_drawing && bitmap) {
      sciter_needs_drawing = false;

      RECT clientRC = {0,0,bitmap->w,bitmap->h};
      SciterProcX(window, SCITER_X_MSG_PAINT(clientRC));
      
      // Clear the screen
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black
      SDL_RenderClear(renderer);

      SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, bitmap);

      SDL_RenderTexture(renderer, texture, nullptr, nullptr); // Render the texture

      SDL_DestroyTexture(texture);

      // Render your game objects here
      //SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red
      //SDL_RenderLine(renderer, 100, 100, 700, 500);

      // Present the rendered frame
      SDL_RenderPresent(renderer);

    }

  }

  SciterProcX(window, SCITER_X_MSG_DESTROY());

  SDL_DestroySurface(bitmap);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}

// Sciter's 

UINT on_load_data(LPSCN_LOAD_DATA pnmld) {

  // your custom loader is here

  LPCBYTE pb = 0; UINT cb = 0;
  aux::wchars wu = aux::chars_of(pnmld->uri);

  if (wu.like(WSTR("this://app/*"))) {
    // try to get them from archive first
    aux::bytes adata = sciter::archive::instance().get(wu.start + 11);
    if (adata.length)
      SciterDataReady(pnmld->hwnd, pnmld->uri, adata.start, adata.length);
  }
  return LOAD_OK;
}

UINT on_data_loaded(LPSCN_DATA_LOADED pnm) { return 0; }

UINT attach_behavior(LPSCN_ATTACH_BEHAVIOR lpab) {
  // attach native behaviors (if we have any)
  sciter::event_handler* pb = sciter::behavior_factory::create(lpab->behaviorName, lpab->element);
  if (pb) {
    lpab->elementTag = pb;
    lpab->elementProc = sciter::event_handler::element_proc;
    return 1;
  }
  return 0;
}

UINT on_invalidate_rect(LPSCN_INVALIDATE_RECT pnm) {
  sciter_needs_drawing = true;
  return 0;
}

UINT on_set_cursor(LPSCN_SET_CURSOR pnm) {

  static SDL_Cursor* arrow_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT);
  static SDL_Cursor* ibeam_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_TEXT);
  static SDL_Cursor* cross_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR);
  static SDL_Cursor* hand_cursor  = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_POINTER);
  static SDL_Cursor* hresize_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_EW_RESIZE);
  static SDL_Cursor* vresize_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NS_RESIZE);

  switch (pnm->cursorId) {
    case CURSOR_ARROW:  SDL_SetCursor(arrow_cursor); break;
    case CURSOR_IBEAM:  SDL_SetCursor(ibeam_cursor); break;
    case CURSOR_CROSS:  SDL_SetCursor(cross_cursor); break;
    case CURSOR_HAND:   SDL_SetCursor(hand_cursor); break;
    case CURSOR_SIZEWE: SDL_SetCursor(hresize_cursor); break;
    case CURSOR_SIZENS: SDL_SetCursor(vresize_cursor); break;
    default: SDL_SetCursor(arrow_cursor); break;
  }
  return 0;
}

// notifiaction cracker
UINT SC_CALLBACK handle_notification(LPSCITER_CALLBACK_NOTIFICATION pnm, LPVOID callbackParam)
{
  // Crack and call appropriate method
  // here are all notifiactions
  switch (pnm->code) {
  case SC_LOAD_DATA: return on_load_data((LPSCN_LOAD_DATA)pnm);
  case SC_DATA_LOADED: return on_data_loaded((LPSCN_DATA_LOADED)pnm);
  case SC_ATTACH_BEHAVIOR: return attach_behavior((LPSCN_ATTACH_BEHAVIOR)pnm);
  case SC_INVALIDATE_RECT: return on_invalidate_rect((LPSCN_INVALIDATE_RECT)pnm);
  case SC_SET_CURSOR: return on_set_cursor((LPSCN_SET_CURSOR)pnm);
  case SC_ENGINE_DESTROYED: break;
  }
  return 0;
}


void dispatch_event(SDL_Window* window, SDL_Renderer* renderer, SDL_Event& event) {

  auto translate = [](SDL_MouseButtonFlags bt) -> MOUSE_BUTTONS {
    unsigned  rv = (0);
    if (bt & SDL_BUTTON_LMASK) rv |= MAIN_MOUSE_BUTTON;
    if (bt & SDL_BUTTON_RMASK) rv |= PROP_MOUSE_BUTTON;
    if (bt & SDL_BUTTON_MMASK) rv |= MIDDLE_MOUSE_BUTTON;
    return MOUSE_BUTTONS(rv);
  };

  auto keyboard_states = []() -> KEYBOARD_STATES {
    SDL_Keymod km = SDL_GetModState();
    return KEYBOARD_STATES(km);
  };

  SDL_ConvertEventToRenderCoordinates(renderer, &event);

  switch (event.type) {
    case SDL_EVENT_WINDOW_RESIZED:           /**< Window has been resized to data1xdata2 */
    {
      resize_bitmap(window);
      break;
    }
    case SDL_EVENT_MOUSE_MOTION:
    {
      SciterProcX(window,
       SCITER_X_MSG_MOUSE(MOUSE_MOVE, translate(event.motion.state), keyboard_states(), POINT{ (INT)event.motion.x, (INT)event.motion.y }));
      break;
    }
    case SDL_EVENT_KEY_DOWN:
      break;

    // ...
  }


}
