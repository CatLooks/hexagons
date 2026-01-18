#pragma once

// include dependencies
#include "dev_panel.hpp"

namespace dev {
	/// Move info generator.
	struct Move {
		/// Emits arguments for developer panel move section.
		/// 
		/// @param section Target section.
		/// @param list Target argument list.
		virtual void emitDev(Section* section, ui::Text::List& list) const;
	};
};