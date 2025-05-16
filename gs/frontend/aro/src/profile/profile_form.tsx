import { useState } from "react";
import type { ProfileDataResponse } from "./profile_data.ts";
import { type SubmitHandler, useForm } from "react-hook-form";
import { updateProfile } from "./profile_api.ts";

const ProfileForm = (props: ProfileDataResponse) => {
  const [isEdit, setIsEdit] = useState(false);
  const {
    register,
    handleSubmit,
    formState: { errors },
  } = useForm<ProfileDataResponse>();

  // TODO: Submit form to backend
  const onSubmit: SubmitHandler<ProfileDataResponse> = (
    data: ProfileDataResponse,
  ) => {
    console.log(data);
    setIsEdit(false);
    try {
      updateProfile(data);
    } catch (error) {
      console.error(error);
    }
  };

  // TODO: Add better error handling
  return (
    <>
      <form onSubmit={handleSubmit(onSubmit)}>
        <label>First Name:</label>
        <input
          type="text"
          defaultValue={props.data.first_name}
          {...register("data.first_name", { required: true })}
          readOnly={!isEdit}
        />
        {errors.data?.first_name && <span>Name is required</span>}

        <label>Last Name:</label>
        <input
          type="text"
          defaultValue={props.data.last_name}
          {...register("data.last_name")}
          readOnly={!isEdit}
        />

        <label>Email:</label>
        <input
          type="text"
          defaultValue={props.data.email}
          readOnly={true}
        />
        <label>Phone:</label>

        <input
          type="text"
          defaultValue={props.data.phone_number}
          {...register("data.phone_number", { required: true, maxLength: 10 })}
          readOnly={!isEdit}
        />
        {errors.data?.phone_number && <span>Phone is required</span>}

        <label>Call Sign:</label>
        <input
          type="text"
          defaultValue={props.data.call_sign}
          {...register("data.call_sign", {
            required: true,
            pattern: /^[A-Za-z0-9]{5,6}$/,
          })}
          readOnly={!isEdit}
        />
        {errors.data?.call_sign && (
          <span>
            Call sign must be 5-6 characters long and contain only letters and
            numbers
          </span>
        )}

        <button type="submit" hidden={!isEdit}>
          Submit
        </button>
      </form>
      <button type="button" hidden={isEdit} onClick={() => setIsEdit(true)}>
        {"Edit"}
      </button>
    </>
  );
};

export default ProfileForm;
