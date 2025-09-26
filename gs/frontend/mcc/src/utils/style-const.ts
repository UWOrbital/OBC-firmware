/**
 * Styling constants for the MCC frontend - colors and font
 */

const COLORS = {
  // Background colors
  DEFAULT_BACKGROUND_COLOR: "#040910",

  // Text colors
  DEFAULT_FONT_COLOR: "#FFFFFF",
};

const FONT = {
  DEFAULT_FONT: "Jaldi",
};

const FONT_WEIGHT = {
  DEFAULT_FONT_WEIGHT: "400",
  BOLD_FONT_WEIGHT: "700",
};

// exported CSS variables for use in html styling
export const CSS_VARIABLES = {
  "--color-background": COLORS.DEFAULT_BACKGROUND_COLOR,
  "--color-font": COLORS.DEFAULT_FONT_COLOR,
  "--font-default": FONT.DEFAULT_FONT,
  "--font-weight-default": FONT_WEIGHT.DEFAULT_FONT_WEIGHT,
  "--font-weight-bold": FONT_WEIGHT.BOLD_FONT_WEIGHT,
};
