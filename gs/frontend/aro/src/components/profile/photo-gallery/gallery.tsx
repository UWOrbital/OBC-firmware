import * as React from "react";
import GallerySection from "./GallerySection";
import { pinnedSlides, getSortedMonthSections, monthSections } from "./galleryData";

export default function Gallery() {
  const [order, setOrder] = React.useState<"asc" | "desc">("desc");

  // Group month sections by year in case of future expansion
  const groupedByYear = React.useMemo(() => {
    const groups: Record<number, typeof monthSections> = {};
    monthSections.forEach((m) => {
      groups[m.year] = groups[m.year] ? [...groups[m.year], m] : [m];
    });
    return groups;
  }, []);

  return (
    <div className="w-full max-w-screen-xl mx-auto px-4 mt-24">
      {/* Card container: make inner content scrollable while keeping pinned & controls fixed at top */}
      <div className="bg-white rounded-2xl shadow-lg border border-gray-100 overflow-hidden pt-10 pb-6 px-10 relative flex flex-col">
        {/* Top row: Pinned title + sort toggle */}
        <div className="mb-4 flex items-center justify-between shrink-0">
          <h2 className="text-2xl font-bold text-gray-900">Pinned</h2>
          <div className="inline-flex rounded-lg border border-gray-300 bg-white shadow-sm p-1">
            <button
              type="button"
              onClick={() => setOrder("asc")}
              className={`px-3 py-1 text-sm rounded-md transition-colors ${
                order === "asc"
                  ? "bg-gray-900 text-white"
                  : "text-gray-700 hover:bg-gray-100"
              }`}
            >
              Ascending
            </button>
            <button
              type="button"
              onClick={() => setOrder("desc")}
              className={`ml-1 px-3 py-1 text-sm rounded-md transition-colors ${
                order === "desc"
                  ? "bg-gray-900 text-white"
                  : "text-gray-700 hover:bg-gray-100"
              }`}
            >
              Descending
            </button>
          </div>
        </div>

        {/* Scrollable area for pinned + year groups. Height accounts for card padding & top controls */}
        <div className="overflow-y-auto mt-2 pr-4 md:pr-6" style={{ maxHeight: "calc(100vh - 11rem)" }}>
          {/* Pinned section without duplicate heading */}
          <GallerySection
            title="Pinned"
            hideTitle
            photos={pinnedSlides}
            targetRowHeight={90}
            spacing={16}
            showDivider
          />

          {Object.keys(groupedByYear)
            .sort((a, b) => (order === "asc" ? Number(a) - Number(b) : Number(b) - Number(a)))
            .map((yearKey) => {
              const year = Number(yearKey);
              const sortedMonths = getSortedMonthSections(order).filter((m) => m.year === year);
              return (
                <div key={year} className="mt-10">
                  <h2 className="text-3xl font-bold text-gray-900 mb-4">{year}</h2>
                  {sortedMonths.map((m, idx) => (
                    <GallerySection
                      key={m.title}
                      title={m.title}
                      photos={m.slides}
                      targetRowHeight={90}
                      spacing={16}
                      showDivider={idx !== sortedMonths.length - 1}
                    />
                  ))}
                </div>
              );
            })}
        </div>
      </div>
    </div>
  );
}
