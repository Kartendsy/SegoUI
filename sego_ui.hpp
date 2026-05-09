/*
 * SegoUI.hpp - Header-only UI Library for SDL3 & GLM
 * Copyright (c) 2026 kartendsy
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_surface.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <cstdint>
#include <glm/ext/scalar_uint_sized.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float4.hpp>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <string>
#include <vector>


using Vec2 = glm::vec2;
using Vec4 = glm::vec4;


inline bool IsInRect(Vec2 mPos, Vec2 gPos, Vec2 size) {
    return (mPos.x >= gPos.x && mPos.x <= gPos.x + size.x && mPos.y >= gPos.y && mPos.y <= gPos.y + size.y);
}

inline uint32_t hash_id(const std::string& name) {
    uint32_t hash = 2166136261u;
    for (char c : name) {
        hash ^= (uint8_t)c;
        hash *= 16777619u;
    }

    return hash;
}

class UINode {
    public:
        uint32_t id;
        std::string name;
        Vec2 position;
        Vec2 size;
        UINode* parent = nullptr;
        std::vector<UINode*> children;
        bool visible = true;

        UINode(const std::string& node_name, Vec2 pos, Vec2 sz) : name(node_name), position(pos), size(sz) {
            this->id = hash_id(node_name);
        }

        virtual ~UINode() {
            for (auto child : children) delete child;
        }


        inline Vec2 get_global_pos() const {
            if (!parent) return position;
            return parent->get_global_pos() + position;
        }

        inline void add_child(UINode* child) {
            child->parent = this;
            children.push_back(child);
        }

        virtual void update(Vec2 mPos, bool mDown) {
            if(!visible) return;
            for (auto child : children) child->update(mPos, mDown);
        }

        virtual void draw(SDL_Renderer* renderer) {
            if(!visible) return;
            for (auto child : children) child->draw(renderer);
        }

};

// ui context

struct UIContext {
    TTF_Font* font = nullptr;
    uint32_t hotID = 0;
    uint32_t activeID = 0;
    Vec2 mousePos;
    bool mouseDown;

    static UIContext& get() {
        static UIContext instance;
        return instance;
    }

    bool init_font(const char* path, float size) {
        if (!TTF_Init()) {
            SDL_Log("Error: %s", SDL_GetError());
            return false;
        }
        font = TTF_OpenFont(path, size);
        return font != nullptr;
    }
};


// components (panel & button)

class UIPanel : public UINode {
    public:
        Vec4 color = {0.18f, 0.18f, 0.18f, 1.0f};

        UIPanel(const std::string& node_name, glm::vec2 pos, glm::vec2 sz) : UINode(node_name, pos, sz) {}

        void draw(SDL_Renderer* renderer) override {
            if (!visible) return;
            Vec2 gPos = get_global_pos();
            SDL_FRect rect = {gPos.x, gPos.y, size.x, size.y};

            SDL_SetRenderDrawColor(renderer, (Uint8)(color.r*255), (Uint8)(color.g*255), (Uint8)(color.b*255), (Uint8)(color.a*255));
            SDL_RenderFillRect(renderer, &rect);

            SDL_SetRenderDrawColor(renderer, 80,80,80,255);
            SDL_RenderRect(renderer, &rect);

            UINode::draw(renderer);
        }
};


class VBoxContainer : public UINode {
    public:
        float separation = 5.0f;

        VBoxContainer(const std::string& name, Vec2 pos) : UINode(name, pos, {0, 0}) {}

        void update(Vec2 mPos, bool mDown) override {
            if(!visible) return;

            float currentY = 0;
            float maxWidth = 0;

            for (auto child : children) {
                if (!child->visible) continue;

                child->position = Vec2(0, currentY);

                currentY += child->size.y + separation;

                if (child->size.x > maxWidth) maxWidth = child->size.x;
            }

            this->size = Vec2(maxWidth, currentY - separation);

            UINode::update(mPos, mDown);
        }
};


class HBoxContainer : public UINode {
    public:

        float separation = 5.0f;

        HBoxContainer(const std::string& name, Vec2 pos) : UINode(name, pos, {0, 0}) {}

        void update(Vec2 mPos, bool mDown) override {
            if(!visible) return;

            float currentX = 0;
            float maxHeight = 0;

            for (auto child : children) {
                if (!child->visible) return;

                child->position = Vec2(currentX, 0);
                currentX += child->size.x + separation;

                if (child->size.y > maxHeight) maxHeight = child->size.y;
            }

            this->size = Vec2(currentX - separation, maxHeight);
            UINode::update(mPos, mDown);
        }
};

class UIButton : public UINode {
    private:
        SDL_Texture* textTexture = nullptr;
        float textW = 0, textH = 0;

    public:
        std::string label;
        bool wasClicked = false;
        Vec4 baseColor = {0.25f, 0.25f, 0.25f, 1.0f};

        UIButton(const std::string& labelText, Vec2 pos, Vec2 sz) : UINode(labelText, pos, sz), label(labelText) {}

        ~UIButton() {
            if (textTexture) SDL_DestroyTexture(textTexture);
        }

        void generate_text(SDL_Renderer* renderer) {
            auto& ctx = UIContext::get();
            if(!ctx.font) return;

            if(textTexture) SDL_DestroyTexture(textTexture);

            SDL_Color white = {255,255,255,255};
            SDL_Surface* surf = TTF_RenderText_Blended(ctx.font, label.c_str(), 0, white);
            if (surf) {
                textW = (float)surf->w;
                textH = (float)surf->h;
                textTexture =SDL_CreateTextureFromSurface(renderer, surf);
                SDL_DestroySurface(surf);
            }
        }

        void update(Vec2 mPos, bool mDown) override {
            if(!visible) return;
            wasClicked = false;

            Vec2 gPos = get_global_pos();
            bool over = IsInRect(mPos, gPos, size);

            auto& ctx = UIContext::get();

            if (over) {
                ctx.hotID = id;
                if (ctx.activeID == 0 && mDown) ctx.activeID = id;
            }

            if (ctx.activeID == id && !mDown) {
                if(ctx.hotID == id) wasClicked = true;
                ctx.activeID = 0;
            }

            UINode::update(mPos, mDown);
        }

        void draw(SDL_Renderer* renderer) override {
            if(!visible) return;

            if(!textTexture) generate_text(renderer);

            auto& ctx = UIContext::get();
            Vec2 gPos = get_global_pos();
            SDL_FRect rect = {gPos.x, gPos.y, size.x, size.y};

            if (ctx.hotID == id) {
                SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
                if (ctx.activeID == id) SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
            } else {
                SDL_SetRenderDrawColor(renderer, (Uint8)(baseColor.r*255), (Uint8)(baseColor.g*255), (Uint8)(baseColor.b*255), 255);
            }

            SDL_RenderFillRect(renderer, &rect);
            SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
            SDL_RenderRect(renderer, &rect);

            if (textTexture) {
                SDL_FRect textRect = {
                    gPos.x + (size.x - textW) * 0.5f,
                    gPos.y + (size.y - textH) * 0.5f,
                    textW,textH
                };
                SDL_RenderTexture(renderer, textTexture, NULL, &textRect);
            }

            UINode::draw(renderer);
        }
};
