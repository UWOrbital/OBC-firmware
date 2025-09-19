import background from "../assets/earth_compressed.webp";

function Background() {
  return (
    <img
      src={background}
      alt="background image"
      className="absolute -bottom-20 -z-10 opacity-40"
    />
  );
}

export default Background;
