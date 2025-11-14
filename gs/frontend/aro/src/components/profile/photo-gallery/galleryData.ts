import spaceImage from "../../../assets/space_dummy_image.png";

export type Slide = {
  src: string;
  width: number;
  height: number;
};

export function generateSlides(src: string, count = 20): Slide[] {
  const slides: Slide[] = [];
  for (let i = 0; i < count; i++) {
    // Alternate orientations to create visual variety in rows
    const mod = i % 3;
    if (mod === 0) slides.push({ src, width: 1600, height: 1066 }); // landscape
    else if (mod === 1) slides.push({ src, width: 1066, height: 1600 }); // portrait
    else slides.push({ src, width: 1200, height: 1200 }); // square
  }
  return slides;
}

export const pinnedSlides = generateSlides(spaceImage, 20);

export type MonthSection = {
  title: string;
  year: number;
  month: number; // 1-12
  slides: Slide[];
};

export const monthSections: MonthSection[] = [
  { title: "October 2025", year: 2025, month: 10, slides: generateSlides(spaceImage, 20) },
  { title: "November 2025", year: 2025, month: 11, slides: generateSlides(spaceImage, 20) },
];

export function getSortedMonthSections(order: "asc" | "desc" = "desc"): MonthSection[] {
  return [...monthSections].sort((a, b) =>
    order === "asc" ? a.year - b.year || a.month - b.month : b.year - a.year || b.month - a.month
  );
}
