# simple tests ran in the directory one level above aro_wrapper

# from aro_wrapper.aro_user_auth_token_wrapper import  *
# from aro_wrapper.aro_user_data_wrapper import  *
# from aro_wrapper.aro_user_login_wrapper import  *
# from gs.backend.data.enums.aro_auth_token import AROAuthToken
# import random
# from datetime import datetime, timedelta


# if __name__ == "__main__":
#     new_user = add_user(call_sign="KEVWAN", email="kevian@gmail.com",
#                         f_name="kevin", l_name="wan", phone_number="8888888888")
#     login = add_login(new_user.email, "lahahhaha", "kevalgo", new_user.id, "abbababababa")
#     print(login)
#     random_days = random.randint(1, 30)
#     random_hours = random.randint(0, 23)
#     random_minutes = random.randint(0, 59)

#     expiry = datetime.now() + timedelta(days=random_days,
#                                         hours=random_hours,
#                                         minutes=random_minutes)

#     user_id = new_user.id
#     print(type(user_id))
#     add_auth_token(token="abc-efg-hid", user_data_id=user_id, expiry=expiry, auth_type=AROAuthToken.DUMMY)
#     print(get_all_auth_tokens())
