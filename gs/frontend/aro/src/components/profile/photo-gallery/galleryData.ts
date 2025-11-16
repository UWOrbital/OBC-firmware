import spaceImage from "../../../assets/space_dummy_image.png";
import type { Slide } from "./GallerySection";

export function generateSlides(src: string, count = 20): Slide[] {
  const slides: Slide[] = [];

  for (let i = 0; i < count; i++) {
    slides.push({
      src,
      width: 320,
      height: 240,
      timestamp: "Tuesday - Nov 15, 2025 - 10:30 PM",
      location: "43.4706° N, -80.5450° W",
    });
  }
  return slides;
}

export const pinnedSlides = generateSlides(spaceImage, 19);

export type MonthSection = {
  title: string;
  year: number;
  month: number;
  slides: Slide[];
};

export const monthSections: MonthSection[] = [
  { title: "October 2025", year: 2025, month: 10, slides: generateSlides(spaceImage, 19) },
  { title: "November 2025", year: 2025, month: 11, slides: generateSlides(spaceImage, 19) },
];

export function getSortedMonthSections(order: "asc" | "desc" = "desc"): MonthSection[] {
  return [...monthSections].sort((a, b) =>
    order === "asc" ? a.year - b.year || a.month - b.month : b.year - a.year || b.month - a.month
  );
}
