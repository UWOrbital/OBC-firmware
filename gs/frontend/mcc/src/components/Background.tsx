import background from "../assets/earth_compressed.webp";

/**
 * @brief Background component displaying the Earth image
 * @return tsx element of Background component
 */
function Background() {
  return (
    <img
      src={background}
      alt="background-image"
      className="fixed bottom-0 left-0 w-full object-cover -z-10 opacity-40"
    />
  );
}

export default Background;
