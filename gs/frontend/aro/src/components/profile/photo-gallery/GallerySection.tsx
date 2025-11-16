import * as React from "react";
import { ChevronLeft, ChevronRight, Download } from "lucide-react";

export type Slide = {
  src: string;
  width: number;
  height: number;
  timestamp?: string;
  location?: string;
};

interface GallerySectionProps {
  title: string;
  photos: Slide[];
  targetRowHeight?: number;
  spacing?: number;
}

export default function GallerySection({
  title,
  photos,
  spacing = 12,
}: GallerySectionProps) {
  const [index, setIndex] = React.useState<number | null>(null);
  const [zoom, setZoom] = React.useState(0.5);

  const closeLightbox = React.useCallback(() => {
    setIndex(null);
    setZoom(0.5);
  }, []);

  const handleZoomIn = () => setZoom((prev) => Math.min(prev + 0.25, 3));
  const handleZoomOut = () => setZoom((prev) => Math.max(prev - 0.25, 0.5));

  const handleDownload = () => {
    if (index === null) return;
    const link = document.createElement("a");
    link.href = photos[index].src;
    link.download = `image-${index + 1}.jpg`;
    link.click();
  };

  React.useEffect(() => {
    const handleKeyDown = (e: KeyboardEvent) => {
      if (e.key === "Escape" && index !== null) {
        closeLightbox();
      }
    };
    window.addEventListener("keydown", handleKeyDown);
    return () => window.removeEventListener("keydown", handleKeyDown);
  }, [index, closeLightbox]);

  return (
    <section className="pt-4 pb-6">
      <div className="flex flex-col items-center w-full">
        <div className="mb-4 w-full flex justify-center">
          <h3 className="text-2xl font-semibold text-gray-900 text-center">{title}</h3>
        </div>
        <div
          className="flex flex-wrap justify-center"
          style={{ gap: typeof spacing === "number" ? `${spacing}px` : "12px" }}
        >
          {photos.map((p, i) => (
            <button
              key={`${p.src}-${i}`}
              type="button"
              onClick={() => setIndex(i)}
              className="w-[200px] h-[150px] overflow-hidden rounded-lg bg-gray-100 shadow-sm hover:shadow-lg transition-all duration-300 cursor-pointer focus:outline-none focus:ring-2 focus:ring-gray-300"
              style={{ flex: "0 0 auto" }}
              aria-label={`Open image ${i + 1}`}
            >
              <img
                src={p.src}
                className="w-full h-full object-cover transition-transform duration-300 hover:scale-110"
                width={200}
                height={150}
                loading="lazy"
              />
            </button>
          ))}
        </div>
      </div>

      {index !== null && (
        <div className="absolute inset-0 z-30 bg-black flex flex-col items-center justify-center py-16 px-8">

          <div className="absolute top-4 right-4 flex gap-2">
            <button
              type="button"
              onClick={handleZoomIn}
              className="rounded-full border border-gray-600 bg-gray-800 p-2 shadow hover:bg-gray-700 transition-colors"
              aria-label="Zoom in"
            >
              <svg className="w-5 h-5" fill="none" stroke="white" viewBox="0 0 24 24">
                <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M21 21l-6-6m2-5a7 7 0 11-14 0 7 7 0 0114 0zM10 7v6m3-3H7" />
              </svg>
            </button>
            <button
              type="button"
              onClick={handleZoomOut}
              className="rounded-full border border-gray-600 bg-gray-800 p-2 shadow hover:bg-gray-700 transition-colors"
              aria-label="Zoom out"
            >
              <svg className="w-5 h-5" fill="none" stroke="white" viewBox="0 0 24 24">
                <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M21 21l-6-6m2-5a7 7 0 11-14 0 7 7 0 0114 0zM13 10H7" />
              </svg>
            </button>
            <button
              type="button"
              onClick={closeLightbox}
              className="rounded-full border border-gray-600 bg-gray-800 p-2 shadow hover:bg-gray-700 transition-colors"
              aria-label="Close"
            >
              <svg className="w-5 h-5" fill="none" stroke="white" viewBox="0 0 24 24">
                <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M6 18L18 6M6 6l12 12" />
              </svg>
            </button>
          </div>

          <div className="flex items-center justify-center w-full gap-4" style={{ height: "calc(100vh - 250px)", maxHeight: "calc(100vh - 250px)", paddingTop: "60px" }}>
            <button
              type="button"
              onClick={() => setIndex((prev) => (prev === null ? null : prev > 0 ? prev - 1 : photos.length - 1))}
              className="hover:bg-gray-900/50 p-4 transition-colors flex items-center justify-center"
              style={{ width: "60px", height: "60px" }}
              aria-label="Previous image"
            >
              <ChevronLeft className="w-8 h-8" color="white" />
            </button>

            <div
              className="flex items-center justify-center overflow-auto cursor-move lightbox-scroll h-full"
              style={{ maxWidth: "calc(100% - 160px)" }}
            >
              <img
                src={photos[index].src}
                alt=""
                className="object-contain rounded-lg shadow-2xl transition-transform duration-200"
                style={{
                  transform: `scale(${zoom})`,
                  minHeight: "75vh",
                  minWidth: "90vw"
                }}
              />
            </div>

            <button
              type="button"
              onClick={() => setIndex((prev) => (prev === null ? null : prev < photos.length - 1 ? prev + 1 : 0))}
              className="hover:bg-gray-900/50 p-4 transition-colors flex items-center justify-center"
              style={{ width: "60px", height: "60px" }}
              aria-label="Next image"
            >
              <ChevronRight className="w-8 h-8" color="white" />
            </button>
          </div>

          <div className="flex items-center justify-between w-full max-w-3xl bg-gray-900/90 rounded-lg px-8 py-4 shadow mb-8">
            <div className="flex flex-col gap-2 text-base text-white">
              {photos[index].timestamp && (
                <div className="flex items-center gap-3">
                  <svg className="w-5 h-5" fill="none" stroke="white" viewBox="0 0 24 24">
                    <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M12 8v4l3 3m6-3a9 9 0 11-18 0 9 9 0 0118 0z" />
                  </svg>
                  <span>{photos[index].timestamp}</span>
                </div>
              )}
              {photos[index].location && (
                <div className="flex items-center gap-3">
                  <svg className="w-5 h-5" fill="none" stroke="white" viewBox="0 0 24 24">
                    <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M17.657 16.657L13.414 20.9a1.998 1.998 0 01-2.827 0l-4.244-4.243a8 8 0 1111.314 0z" />
                    <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M15 11a3 3 0 11-6 0 3 3 0 016 0z" />
                  </svg>
                  <span>{photos[index].location}</span>
                </div>
              )}
            </div>
            <button
              type="button"
              onClick={handleDownload}
              className="rounded-full border border-gray-600 bg-gray-800 p-3 shadow hover:bg-gray-700 transition-colors"
              aria-label="Download"
            >
              <Download className="w-6 h-6" color="white" />
            </button>
          </div>
        </div>
      )}
    </section>
  );
}
