import logo from "../assets/aro_logo.png";
import { Button } from "./ui/button";

/**
 * @brief Hero component displaying the ARO hero page
 * @return tsx element of Logo component
*/

function Logo() {
    return (
        <div className="pt-8 w-full flex flex-col items-center">
            <img
             src={logo}
             alt="aro-logo"
             className="w-2xs md:w-xs lg:w-xs"></img>
            <h1 className="font-[Raleway] font-bold -mt-9 text-4xl md:text-6xl lg:text-7xl">Amateur Radio Operator</h1>
            <p className="font-[Raleway] font-light mt-4 text-1xl md:text-2xl lg:text-3xl">Please login to make a request.</p>
            <div className="flex gap-50 mt-15x md:mt-10 lg:mt-10">
                <Button size="lg" variant="outline" className="bg-transparent font-[Raleway] font-bold">Sign Up</Button>
                <Button size="lg" variant="outline" className="bg-transparent font-[Raleway] font-bold">Login</Button>
            </div>
        </div>
    )
}

export default Logo