import UserTable from "./UserTable";
import { SampleUsers } from "./SampleUsers";
export default function AROUserAdmin() {
    return (
        <>
            <UserTable data={SampleUsers}/>
        </>
    )
}