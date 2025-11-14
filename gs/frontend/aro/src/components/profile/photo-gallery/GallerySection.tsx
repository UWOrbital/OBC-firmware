import * as React from "react";

export type Slide = {
  src: string;
  width: number;
  height: number;
};

interface GallerySectionProps {
  title: string;
  photos: Slide[];
  hideTitle?: boolean; // used for pinned to avoid duplicate heading
  targetRowHeight?: number; // no-op in fixed grid (kept for compatibility)
  spacing?: number; // spacing between photos (px)
  showDivider?: boolean; // show bottom divider
}

/**
 * Renders a titled gallery section. Clicking a photo opens a simple lightbox
 * overlay that covers the entire white card (the nearest positioned ancestor).
 */
export default function GallerySection({
  title,
  photos,
  hideTitle = false,
  spacing = 12,
  showDivider = true,
}: GallerySectionProps) {
  const [index, setIndex] = React.useState<number | null>(null);

  return (
    <section className="py-8">
      {!hideTitle && (
        <div className="mb-4">
          <h3 className="text-lg font-semibold text-gray-900">{title}</h3>
        </div>
      )}

      {/* Fixed-size grid: each photo rendered at 320x240 */}
      <div
        className="flex flex-wrap justify-center"
        style={{ gap: typeof spacing === "number" ? `${spacing}px` : "12px" }}
      >
        {photos.map((p, i) => (
          <button
            key={`${p.src}-${i}`}
            type="button"
            onClick={() => setIndex(i)}
            className="w-[240px] h-[180px] overflow-hidden rounded-lg bg-gray-100 shadow-sm hover:shadow transition-shadow cursor-pointer focus:outline-none focus:ring-2 focus:ring-gray-300"
            style={{ flex: "0 0 auto" }}
            aria-label={`Open image ${i + 1}`}
          >
            <img
              src={p.src}
              alt=""
              className="w-full h-full object-cover"
              width={240}
              height={180}
              loading="lazy"
            />
          </button>
        ))}
      </div>

      {index !== null && (
        <div className="absolute inset-0 z-30 bg-white/95 backdrop-blur-sm flex items-center justify-center">
          <button
            type="button"
            onClick={() => setIndex(null)}
            className="absolute top-4 right-4 rounded-full border border-gray-300 bg-white px-3 py-1 text-sm shadow hover:bg-gray-50"
          >
            Close
          </button>
          <img
            src={photos[index].src}
            alt=""
            className="max-h-[90%] max-w-[90%] object-contain rounded-lg shadow-2xl"
          />
        </div>
      )}

      {showDivider && <div className="mt-8 border-t border-gray-200" />}
    </section>
  );
}
