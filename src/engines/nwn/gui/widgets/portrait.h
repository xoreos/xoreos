/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names
 * can be found in the AUTHORS file distributed with this source
 * distribution.
 *
 * xoreos is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * xoreos is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with xoreos. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file
 *  A portrait model and widget.
 */

#ifndef ENGINES_NWN_GUI_WIDGETS_PORTRAIT_H
#define ENGINES_NWN_GUI_WIDGETS_PORTRAIT_H

#include <vector>

#include "src/graphics/guielement.h"

#include "src/graphics/aurora/texturehandle.h"

#include "src/engines/nwn/gui/widgets/nwnwidget.h"

#include "src/graphics/shader/shaderrenderable.h"

namespace Common {
	class UString;
}

namespace Engines {

namespace NWN {

/** A NWN portrait model. */
class Portrait : public Graphics::GUIElement {
public:
	enum Size {
		kSizeHuge   = 0,
		kSizeLarge     ,
		kSizeMedium    ,
		kSizeSmall     ,
		kSizeTiny      ,
		kSizeIcon      ,
		kSizeMAX
	};

	Portrait(const Common::UString &name, Size size, float border = 0.0f,
	         float bR = 1.0f, float bG = 1.0f, float bB = 1.0f, float bA = 1.0f);
	~Portrait();

	float getWidth () const; ///< Get the scrollbar's width.
	float getHeight() const; ///< Get the scrollbar's height.

	/** Set the current portrait. */
	void setPortrait(const Common::UString &name);

	/** Set border color. */
	void setBorderColor(float bR, float bG, float bB, float bA);

	/** Set the current position of the portrait. */
	void setPosition(float x, float y, float z);

	/** Get the current position of the portrait. */
	void getPosition(float &x, float &y, float &z) const;

	/** Is the point within the portrait? */
	bool isIn(float x, float y) const;

	// Renderable
	void calculateDistance();
	void render(Graphics::RenderPass pass);
	void renderImmediate(const glm::mat4 &parentTransform);

private:
	struct Quad {
		float vX[4], vY[4];
		float tX[4], tY[4];
	};

	Graphics::Aurora::TextureHandle _texture;

	Size _size;

	float _border;
	float _x;
	float _y;
	float _width;
	float _height;

	float _bR;
	float _bG;
	float _bB;
	float _bA;

	Quad _qPortrait;
	std::vector<Quad> _qBorder;

	std::unique_ptr<Graphics::Shader::ShaderSurface> _surface;
	std::unique_ptr<Graphics::Shader::ShaderMaterial> _material;
	std::unique_ptr<Graphics::Shader::ShaderMaterial> _borderMaterial;
	std::unique_ptr<Graphics::Shader::ShaderRenderable> _renderable;
	std::unique_ptr<Graphics::Shader::ShaderRenderable> _borderRenderable;

	void setSize();
	void createBorder();
};

/** A NWN portrait widget. */
class PortraitWidget : public NWNWidget {
public:
	PortraitWidget(::Engines::GUI &gui, const Common::UString &tag,
	               const Common::UString &name, Portrait::Size size, float border = 0.0f,
	               float bR = 1.0f, float bG = 1.0f, float bB = 1.0f, float bA = 1.0f);
	~PortraitWidget();

	void show();
	void hide();

	void mouseDown(uint8_t state, float x, float y);
	void mouseWheel(uint8_t state, int x, int y);

	void setPosition(float x, float y, float z);

	void setPortrait(const Common::UString &name);
	void setBorderColor(float bR, float bG, float bB, float bA);

	float getWidth () const;
	float getHeight() const;

	void setTag(const Common::UString &tag);

private:
	Portrait _portrait;
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_WIDGETS_PORTRAIT_H
