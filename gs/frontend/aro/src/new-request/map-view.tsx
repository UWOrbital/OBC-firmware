import { MapContainer, TileLayer, useMap, Marker, Popup, useMapEvents } from 'react-leaflet'
import 'leaflet/dist/leaflet.css';
import React from "react";


interface MapViewProps {
    latitude: number;
    longitude: number;
    setLatitude: (lat: number | null) => void;
    setLongitude: (lng: number | null) => void;
}

function RecenterMap({ lat, lng }: { lat: number; lng: number }) {
    const map = useMap();
    map.setView([lat, lng]);
    return null;
}

function LocationSelector({
    setLatitude,
    setLongitude,
}: {
    setLatitude: (lat: number | null) => void;
    setLongitude: (lng: number | null) => void;
}) {
    useMapEvents({
        click(e) {
            const event = e.originalEvent as MouseEvent;
            if (event.shiftKey) {
            setLatitude(e.latlng.lat);
            setLongitude(e.latlng.lng);
            }
        },
        });
    return null;
}

const MapView: React.FC<MapViewProps> = ({ latitude, longitude, setLatitude, setLongitude }) => {
    return (
        <MapContainer
            center={[latitude, longitude]}
            zoom={7}
            scrollWheelZoom={false}
            style={{ height: "70vh", width: "100%" }}
            doubleClickZoom={false} 
            boxZoom={false} 
            keyboard={false}
        > 
            <TileLayer
                attribution='&copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a> contributors'
                url="https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png"
            />
            <Marker position={[latitude, longitude]}>
            <Popup>The selected request position</Popup>
            </Marker>
            <LocationSelector setLatitude={setLatitude} setLongitude={setLongitude} />
            <RecenterMap lat={latitude} lng={longitude} />
        </MapContainer>
    );
};

export default MapView;