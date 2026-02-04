import { useRef, useState, type FormEventHandler } from "react";
import "./profile-form.css"

export default function ProfileForm() {
  const [isEditing, setIsEditing] = useState(false);
  const [fileName, setFileName] = useState("");
  const fileInputRef = useRef<HTMLInputElement>(null);

  const handleSubmit: FormEventHandler<HTMLFormElement> = (e) => {
    e.preventDefault();
    console.log(e.currentTarget.callsign.value);
  }

  const handleUpload = () => {
    const fileInput = fileInputRef.current;
      if (!fileInput || !fileInput.files || fileInput.files.length === 0) {
        console.log("Please select a file to upload");
        return;
      }

      // Extract the first file from the file input
      const file = fileInput.files[0];

      // custom file validation, set as needed these are just some examples
      if (file.size >= 5 * 1024 * 1024) {
        console.log("Max 5MB image upload");
        return;
      } else if (!file.type.startsWith("image/")) {
        console.log("Invalid image format");
        return;
      }
      console.log(`${file.name} can be uploaded! (success message TODO)`);
  }

  return (
    <div className="flex w-auto h-[calc(100vh-7rem)] max-h-full max-w-screen justify-center mt-24 mx-4 p-4 overflow-auto">
      <div className="flex flex-wrap items-center justify-evenly max-w-full max-h-full min-h-fit rounded-2xl bg-white gap-4 p-6">

        {/* profile picture left bar */}
        <div className="flex flex-col max-w-full items-center h-fit max-h-full gap-2">
          <div className="flex w-60 max-w-full aspect-square items-center justify-center rounded-lg bg-indigo-950">
            <img src="/vite.svg" alt="profile image" className="size-full object-cover"/>
          </div>
          <label htmlFor="pfp-upload" className="w-full max-w-full p-1 transition-colors hover:bg-gray-200 text-gray-900 border-black border rounded-lg text-center cursor-pointer shadow">
            Edit profile image
          </label>
          <input id="pfp-upload" type="file" hidden ref={fileInputRef} accept="image/*" onChange={(e) => setFileName(e.currentTarget.files?.item(0)?.name ?? "")}/>
          {fileName &&
            <div className="flex flex-wrap items-center justify-center h-fit w-full gap-2">
              <p className="text-gray-900 wrap-break-word">{fileName}</p>
              <button onClick={handleUpload} className="size-fit px-2 py-1 rounded-lg transition-colors bg-gray-900 hover:bg-gray-700 shadow">Upload</button>
            </div>
          }
        </div>

        {/* edit form right bar */}
        <form id="edit-form" onSubmit={handleSubmit} className="flex flex-col items-end w-xl h-fit max-h-full gap-4 ">
          <label className="w-full text-2xl font-medium text-black wrap-break-word">Edit Information</label>

          {/* form text fields */}
          <div className="flex flex-col w-full h-auto gap-2">

            {/* field 1 & 2: first & last name */}
            <div className="flex flex-wrap w-full gap-x-4 gap-y-2">
              <div className="field-label-container grow shrink">
                <label htmlFor="fname-field">First name</label>
                <input id="fname-field" name="firstname" placeholder="First name" disabled={!isEditing}
                className="form-field"/>
              </div>
              <div className="field-label-container grow shrink">
                <label htmlFor="lname-field">Last name</label>
                <input id="lname-field" name="lastname" placeholder="Last name" disabled={!isEditing}
                className="form-field"/>
              </div>
            </div>

            {/* field 3: email */}
            <div className="field-label-container">
              <label htmlFor="email-field">Email</label>
              <input id="email-field" name="email" type="email" placeholder="Email" disabled={!isEditing}
              className="form-field"/>
            </div>

            {/* field 4: phone */}
            <div className="field-label-container">
              <label htmlFor="phone-field">Phone</label>
              <input id="phone-field" name="phone" type="tel" placeholder="Phone" disabled={!isEditing}
              className="form-field"/>
            </div>

            {/* field 5: callsign */}
            <div className="field-label-container">
              <label htmlFor="callsign-field">Callsign</label>
              <input id="callsign-field" name="callsign" placeholder="Callsign" disabled={!isEditing}
              className="form-field"/>
            </div>

          </div>
          <div className="flex flex-wrap size-fit gap-x-3 gap-y-1">
            <button type="submit" className="form-buttons transition-colors bg-gray-900 hover:bg-gray-700 shadow"
            onClick={() => (setIsEditing(prev => !prev))} hidden={!isEditing}>
              Submit
            </button>
            <button type="button" className="form-buttons transition-colors hover:bg-gray-200 border border-solid border-gray-900 text-gray-900 shadow"
            onClick={() => (setIsEditing(prev => !prev))} hidden={isEditing}>
              Edit
            </button>
            <button type="reset" className="form-buttons transition-colors hover:bg-gray-200 border border-solid border-gray-900 text-gray-900 shadow"
            onClick={() => (setIsEditing(prev => !prev))} hidden={!isEditing}>
              Cancel
            </button>
          </div>
        </form>
      </div>
    </div>
  );
}