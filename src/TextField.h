#ifndef TEXT_FIELD_H
#define TEXT_FIELD_H

#include "renderable.h"

#include <memory>
#include <coreutils/vec.h>
#include <grappix/grappix.h>


class TextField : public Renderable {
public:

	TextField() : Renderable(grappix::screenptr), pos(0, 0), scale(1.0), color(0xffffffff), add(0), text(""), tsize(-1, -1) {
	}

	TextField(const grappix::Font &font, const std::string &text = "", float x = 0.0, float y = 0.0, float sc = 1.0, uint32_t col = 0xffffffff) : Renderable(grappix::screenptr), pos(x, y), scale(sc), color(col), add(0), text(text), tsize(-1, -1), font(font) {
	}


	utils::vec2f pos;
	float scale;
	grappix::Color color;
	float add;

	virtual void setText(const std::string &t) {
		text = t;
		tsize.x = -1;
	}

	virtual std::string getText() const { return text; }

	int getWidth() {
		if(tsize.x == -1)
			tsize = font.get_size(text, scale);
		return tsize.x;
	}

	void setFont(const grappix::Font &f) {
		font = f;
	}

	virtual void render(uint32_t delta) override {
		if(color.a == 0.0)
			return;
		auto x = pos.x;
		auto y = pos.y;
		if(tsize.x == -1)
			tsize = font.get_size(text, scale);
		//if(x < 0) x = grappix::screen.width() - tlen + x;
		//if(y < 0) y = grappix::screen.height() + y;
		target->text(font, text, x, y, color + add, scale);
	}

	struct iterator  {
		iterator(TextField *field, int index) : field(field), index(index) {}
		iterator(const iterator& rhs) : field(rhs.field), index(rhs.index) {}

		bool operator!= (const iterator& other) const {
			return index != other.index;
		}

		float& operator* () {
			if(index <4)
				return field->color[index];
			else if(index == 4)
				return field->pos.x;
			else if(index == 5)
				return field->pos.y;
			else if(index == 6)
				return field->scale;
			else
				return field->add;
		}

		const iterator& operator++ () {
			++index;
			return *this;
		}

		TextField *field;
		int index;
	};

	iterator begin() {
		return iterator(this, 0);
	}

	iterator end() {
		return iterator(this, 8);
	}

protected:
	//float* f[8];
	std::string text;
	mutable utils::vec2i tsize;
	grappix::Font font;
};



#endif // TEXT_FIELD_H
